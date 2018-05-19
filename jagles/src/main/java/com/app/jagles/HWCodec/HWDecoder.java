package com.app.jagles.HWCodec;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.SurfaceTexture;
import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaCodecList;
import android.media.MediaExtractor;
import android.media.MediaFormat;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.opengl.Matrix;
import android.os.Build;
import android.util.Log;
import android.view.Surface;

import com.app.jagles.eventbus.GLPlayFrameComeBackEventBus;
import com.app.jagles.listener.HWDecoderCaptureLinstener;
import com.app.jagles.video.GLVideoConnect;
import com.app.jagles.video.GLVideoDisplay;
import com.app.jagles.video.Parameter;

import java.io.BufferedOutputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.Buffer;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.IntBuffer;

import static android.opengl.GLES11Ext.GL_TEXTURE_EXTERNAL_OES;
import static android.opengl.GLES20.GL_CULL_FACE;
import static android.opengl.GLES20.GL_FRAMEBUFFER;
import static android.opengl.GLES20.GL_SCISSOR_TEST;
import static android.opengl.GLES20.GL_VIEWPORT;
import static android.opengl.GLES20.glBindFramebuffer;
import static android.opengl.GLES20.glGetError;
import static android.util.Log.VERBOSE;
import static com.app.jagles.HWCodec.GlUtil.TAG;

/**
 * Created by hongli on 2017/4/20.
 */

public class HWDecoder implements SurfaceTexture.OnFrameAvailableListener {
    public int mOffscreenTexture = 0;
    private int mFramebuffer;
    private int mDepthBuffer;
    public int mWidth = 0;
    public int mHeight = 0;
    private int mDecodeTextureID = 0;
    private SurfaceTexture mSurfacetex;
    private Surface mSurface;
    private FullFrameRect mFullScreen;
    private GLVideoDisplay mView;
    private final float[] mIdentityMatrix;
    private MediaCodec decoder = null;
    private static final boolean VERBOSE = false;
    private int mUpdateSurface = 0;
    public boolean isSupportHardWare;
    private ByteBuffer sps = null;
    private ByteBuffer pps = null;
    private long mTimestamp = 0;
    public boolean mHaveImage = false;
    private boolean mCaptureImage = false;
    private float[] mSTMatrix = new float[16];

    private String mCaptureImageFile = "";
    private HWDecoderCaptureLinstener captureImageListener;
    public int[] mCropInfo = null;

    private MediaCodec.BufferInfo mBufferInfo = new MediaCodec.BufferInfo();

    public boolean IsInitDecoder = false;

    public void setCaptureImageListener(HWDecoderCaptureLinstener captureImageListener) {
        this.captureImageListener = captureImageListener;
    }

    public void SetRatio(int width, int height) {
    }

    public HWDecoder(GLVideoDisplay view) {
        mView = view;
        mIdentityMatrix = new float[16];
        Matrix.setIdentityM(mIdentityMatrix, 0);
        Matrix.setIdentityM(mSTMatrix, 0);
        checkMediaDecoder();
    }

    public void MakeCurrent() {
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, mFramebuffer);
    }

    public void ReleaseCurrent() {
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);
    }

    private int GetSPSPPS(byte[] texture) {
        int len = texture.length > 1024 ? 1024 : texture.length - 4;
        for (int i = 0; i < len; i++) {
            if (texture[i] == 0 && texture[i + 1] == 0 && texture[i + 2] == 1) {
                if ((texture[i + 3] & 0x1f) == 0x07) {
                    for (int j = i + 4; j < len; j++)
                        if (texture[j] == 0 && texture[j + 1] == 0 && texture[j + 2] == 1) {
                            sps = ByteBuffer.allocate(j - i);
                            sps.put(texture, i, j - i);
                            break;
                        }
                }
                if ((texture[i + 3] & 0x1f) == 0x08) {
                    for (int j = i + 5; j < len; j++)
                        if (texture[j] == 0 && texture[j + 1] == 0 && texture[j + 2] == 1) {
                            pps = ByteBuffer.allocate(j - i);
                            pps.put(texture, i, j - i);
                            break;
                        }
                }
                if ((texture[i + 3] & 0x1f) == 0x05)
                    return i;
                if ((texture[i + 3] & 0x1f) == 0x01)
                    return 0;
            }
        }
        return 0;
    }

    public void InitDecoder(int width, int height, byte[] texture) throws IOException {
        mSurfacetex.setDefaultBufferSize(width, height);
        mSurface = new Surface(mSurfacetex);
        String mime;
        mime = "video/avc";
//        mWidth = width;
//        mHeight = height;
        decoder = MediaCodec.createDecoderByType(mime);
        GetSPSPPS(texture);
        Log.i("testplaying", "initDecoder---->width:" + width + ",height:" + height);
        //prepareFBOFramebuffer(width,height);
        MediaFormat fmt = MediaFormat.createVideoFormat(mime, width, height);
        try {
            decoder.configure(fmt, mSurface, null, 0);
            decoder.start();
            MediaFormat fmt1 = decoder.getOutputFormat();

            IsInitDecoder = true;
        } catch (Exception e) {
            e.printStackTrace();
        }
        prepareFramebuffer(width, height);

        //mView.mRender.SetFBOTexture(mView.mRender.mParametricManager,mWorldmapTexture);
    }

    private void checkMediaDecoder() {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.LOLLIPOP)
            return;
        int numCodecs = MediaCodecList.getCodecCount();
        MediaCodecInfo mediaCodecInfo = null;
        for (int i = 0; i < numCodecs && mediaCodecInfo == null; i++) {
            MediaCodecInfo info = MediaCodecList.getCodecInfoAt(i);
            if (info.isEncoder()) {
                continue;
            }
            String[] types = info.getSupportedTypes();
            boolean found = false;
            for (int j = 0; j < types.length && !found; j++) {
                Log.d(TAG, "-------------------hardware codec:" + types[j]);
                if (types[j].equals("video/avc")) {
                    System.out.println("found");
                    found = true;
                    isSupportHardWare = found;
                    mView.mRender.mMaxSupportWidth = info.getCapabilitiesForType(types[j]).getVideoCapabilities().getSupportedWidths().getUpper();
                    mView.mRender.mMaxSupportHeight = info.getCapabilitiesForType(types[j]).getVideoCapabilities().getSupportedHeights().getUpper();
                    Log.i(TAG, "support width == " + info.getCapabilitiesForType(types[j]).getVideoCapabilities().getSupportedWidths().getUpper() + ", height == " + info.getCapabilitiesForType(types[j]).getVideoCapabilities().getSupportedHeights().getUpper() + ",low width:" +
                            info.getCapabilitiesForType(types[j]).getVideoCapabilities().getSupportedWidths().getLower() + ",low height:" + info.getCapabilitiesForType(types[j]).getVideoCapabilities().getSupportedHeights().getLower());
                    return;
                }
            }
            if (!found) {
                continue;
            }
            mediaCodecInfo = info;
        }
        //mediaCodecInfo.getCapabilitiesForType("video/avc").colorFormats);
    }

    public void Release() {
        if (decoder != null) {
            decoder.release();
            decoder = null;
        }
        if (mOffscreenTexture != 0) {
            int[] a = new int[2];
            a[0] = mOffscreenTexture;
            GLES20.glDeleteTextures(1, a, 0);
            mOffscreenTexture = 0;
            a[0] = mFramebuffer;
            GLES20.glDeleteFramebuffers(1, a, 0);
            mFramebuffer = 0;

            a[0] = mDepthBuffer;
            GLES20.glDeleteRenderbuffers(1, a, 0);
            mDepthBuffer = 0;
        }
        if (mDecodeTextureID != 0) {
            int[] a = new int[2];
            a[0] = mDecodeTextureID;
            GLES20.glDeleteTextures(1, a, 0);
            mDecodeTextureID = 0;
        }
        mWidth = 0;
        mHeight = 0;
        IsInitDecoder = false;
    }

    public void InputCompressBuffer(byte[] buf, int buflen) {
        try {
            final int TIMEOUT_USEC = 10000;
            //ByteBuffer[] decoderInputBuffers = decoder.getInputBuffers();
            int inputChunk = 0;
            long firstInputTimeNsec = -1;

            boolean outputDone = false;
            boolean inputDone = false;

            int inputBufIndex = decoder.dequeueInputBuffer(500000);
            if (inputBufIndex >= 0) {
                if (firstInputTimeNsec == -1) {
                    firstInputTimeNsec = System.nanoTime();
                }
                ByteBuffer inputBuf;
                if (Build.VERSION.SDK_INT < Build.VERSION_CODES.LOLLIPOP) {
                    // 从输入队列里去空闲buffer
                    inputBuf = decoder.getInputBuffers()[inputBufIndex];
                    inputBuf.clear();
                } else {
                    // SDK_INT > LOLLIPOP
                    inputBuf = decoder.getInputBuffer(inputBufIndex);
                }
                //ByteBuffer inputBuf = decoderInputBuffers[inputBufIndex];
                // Read the sample data into the ByteBuffer.  This neither respects nor
                // updates inputBuf's position, limit, etc.


                //byte a=0;
                //inputBuf.put(a);
                inputBuf.put(buf);
                decoder.queueInputBuffer(inputBufIndex, 0, buflen,
                        mTimestamp, 0);
                mTimestamp += 33333;
            } else {
                Log.d(TAG, "InputCompressBuffer: ----->mHardwareDecoder:false");
                mView.mRender.mHardwareDecoder = false;
                GLVideoConnect.getInstance().SetHardwareDecoder(mView.msg, mView.mRender.mHardwareDecoder, 0, mView.mRender.mMaxSupportWidth, mView.mRender.mMaxSupportHeight);
                return;
            }

            int decoderStatus = decoder.dequeueOutputBuffer(mBufferInfo, 500000);
            //Log.i("honglee_old", "status === " + decoderStatus + "  size:" + mBufferInfo.size);
            if (decoderStatus == MediaCodec.INFO_TRY_AGAIN_LATER) {
                // no output available yet
                if (VERBOSE) Log.d(TAG, "no output from decoder available");
            } else if (decoderStatus == MediaCodec.INFO_OUTPUT_BUFFERS_CHANGED) {
                // not important for us, since we're using Surface
                if (VERBOSE) Log.d(TAG, "decoder output buffers changed");
            } else if (decoderStatus == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
                MediaFormat newFormat = decoder.getOutputFormat();
                if (VERBOSE) Log.d(TAG, "decoder output format changed: " + newFormat);
            } else if (decoderStatus < 0) {
                throw new RuntimeException(
                        "unexpected result from decoder.dequeueOutputBuffer: " +
                                decoderStatus);
            } else { // decoderStatus >= 0
                if (firstInputTimeNsec != 0) {
                    // Log the delay from the first buffer of input to the first buffer
                    // of output.
                    long nowNsec = System.nanoTime();
                    //Log.d(TAG, "startup lag " + ((nowNsec-firstInputTimeNsec) / 1000000.0) + " ms");
                    firstInputTimeNsec = 0;
                }
                boolean doLoop = false;
                if (VERBOSE) Log.d(TAG, "surface decoder given buffer " + decoderStatus +
                        " (size=" + mBufferInfo.size + ")");
                if ((mBufferInfo.flags & MediaCodec.BUFFER_FLAG_END_OF_STREAM) != 0) {
                    if (VERBOSE) Log.d(TAG, "output EOS");
                }

                decoder.releaseOutputBuffer(decoderStatus, mBufferInfo.size != 0);

                //            mSurfacetex.updateTexImage();
                //Log.d("honglee_old", "decoderdone...................................");
            }
        } catch (Exception e) {
            e.printStackTrace();
            Log.d(TAG, "InputCompressBuffer: ----->mHardwareDecoder:false" + e.toString());
            mView.mRender.mHardwareDecoder = false;
            GLVideoConnect.getInstance().SetHardwareDecoder(mView.msg, mView.mRender.mHardwareDecoder, 0, mView.mRender.mMaxSupportWidth, mView.mRender.mMaxSupportHeight);
        }
    }

    public boolean GetUpdateSurface() {
        synchronized (this) {
            boolean b = mUpdateSurface != 0;
            //if(b)
            mUpdateSurface--;
            return b;
        }
    }

    public void UpdateSurfaceImage() {
        synchronized (this) {
            //Log.d("honglee", "UpdateSurfaceImage...........");
            mSurfacetex.updateTexImage();
            mSurfacetex.getTransformMatrix(mSTMatrix);
        }
    }

    public void CaptureImage(String file) {
        synchronized (this) {
            mCaptureImageFile = file;
            mCaptureImage = true;
        }
    }


    /**
     * 截图代码。。。。
     *
     * @param getCrop
     */
    public void Draw(boolean getCrop) {
        if (!IsInitDecoder)
            return;
        MakeCurrent();
        GLES20.glClearColor(0f, 0f, 0f, 1f);
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);
        GLES20.glDisable(GL_CULL_FACE);
        GLES20.glDisable(GL_SCISSOR_TEST);
        GLES20.glViewport(0, 0, mWidth, mHeight);
        mFullScreen.drawFrame(mDecodeTextureID, mSTMatrix);


        if (getCrop && !mView.mRender.GetDualMode(mView.mRender.mParametricManager) && mView.mRender.mScreenMode != Parameter.SCRN_NORMAL) {
            GLES20.glFlush();
            mCropInfo = mView.mRender.SetCropInfo(mView.mRender.mParametricManager, mView.mRender.conn, mWidth, mHeight);
        }

        Log.d(TAG, "Draw: -------->" + mCaptureImage + " ------" + mHaveImage + "---" + mWidth + "----" + mHeight);
        if (mCaptureImage && mHaveImage && mWidth > 0 && mHeight > 0) {
            mCaptureImage = false;
            int l;
            int t;
            int w;
            int h;
            if (mCropInfo != null) {
                l = mCropInfo[0];
                t = mCropInfo[1];
                w = mCropInfo[2] - mCropInfo[0];
                h = mCropInfo[3] - mCropInfo[1];
            } else {
                l = 0;
                t = 0;
                w = mWidth;
                h = mHeight;
            }
            ByteBuffer buf = ByteBuffer.allocateDirect(w * h * 4);
            buf.order(ByteOrder.LITTLE_ENDIAN);
            GLES20.glReadPixels(l, t, w, h,
                    GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE, buf);
            GlUtil.checkGlError("glReadPixels");
            buf.rewind();
            BufferedOutputStream bos = null;

            FileOutputStream outputStream = null;
            try {
                try {
                    outputStream = new FileOutputStream(mCaptureImageFile);
                    bos = new BufferedOutputStream(outputStream);
                } catch (IOException e) {
                    e.printStackTrace();
                }
                try {
                    Bitmap bmp = Bitmap.createBitmap(w, h, Bitmap.Config.ARGB_8888);
                    bmp.copyPixelsFromBuffer(buf);
                    bmp.compress(Bitmap.CompressFormat.JPEG, 90, bos);
                    bmp.recycle();
                    mView.mRender.OnCaptureImage(1, 0);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            } finally {
                if (bos != null) {
                    try {
                        bos.close();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
                if (outputStream != null) {
                    try {
                        outputStream.close();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
            }
        }

        GLES20.glFlush();
        //mView.RenderBuffer();
        ReleaseCurrent();
    }

    public void InitDecoderSurface() {
        int[] textures = new int[1];
        GLES20.glGenTextures(1, textures, 0);

        mDecodeTextureID = textures[0];
        GLES20.glBindTexture(GL_TEXTURE_EXTERNAL_OES, mDecodeTextureID);
        GlUtil.checkGlError("glBindTexture mTextureID");

        // Can't do mipmapping with camera source
        GLES20.glTexParameterf(GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_MIN_FILTER,
                GLES20.GL_NEAREST);
        GLES20.glTexParameterf(GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_MAG_FILTER,
                GLES20.GL_LINEAR);
        // Clamp to edge is the only option
        GLES20.glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_WRAP_S,
                GLES20.GL_CLAMP_TO_EDGE);
        GLES20.glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_WRAP_T,
                GLES20.GL_CLAMP_TO_EDGE);
        GlUtil.checkGlError("glTexParameteri mTextureID");

        /*
         * Create the SurfaceTexture that will feed this textureID, and pass it to the camera
         */

        mSurfacetex = new SurfaceTexture(mDecodeTextureID);
        mSurfacetex.setOnFrameAvailableListener(this);
        mFullScreen = new FullFrameRect(
                new Texture2dProgram(Texture2dProgram.ProgramType.TEXTURE_EXT));

    }


    @Override
    synchronized public void onFrameAvailable(SurfaceTexture surface) {
        Log.d("honglee", "onFrameAvailable...........");
        mUpdateSurface++;
        if (!mHaveImage) {
            mView.mRender.DoStatus(mView.mRender.mParametricManager, "   ", 0, 0);
            if (GLPlayFrameComeBackEventBus.getInstance().getOnFrameComeBackListener() != null) {
                GLPlayFrameComeBackEventBus.getInstance().getOnFrameComeBackListener().onFrameComeBack(true);
                //TODO:回调帧画面
//        if (CallBackFirstFrameEvent.getInstance().getOnFirstFrameListener() != null) {
//            CallBackFirstFrameEvent.getInstance().getOnFirstFrameListener().onCallBackFirstFrame(mHaveImage);
//        }
            }
        }
        mHaveImage = true;
    }

    public void prepareFramebuffer(int width, int height) {
        //GLES20.glGetError();
        synchronized (this) {
            if (mOffscreenTexture != 0) {
                int[] a = new int[2];
                a[0] = mOffscreenTexture;
                GLES20.glDeleteTextures(1, a, 0);
                mOffscreenTexture = 0;

                a[0] = mFramebuffer;
                GLES20.glDeleteFramebuffers(1, a, 0);
                mFramebuffer = 0;

                a[0] = mDepthBuffer;
                GLES20.glDeleteRenderbuffers(1, a, 0);
                mDepthBuffer = 0;
            }

            GlUtil.checkGlError("prepareFramebuffer start");
            int[] values = new int[1];

            // Create a texture object and bind it.  This will be the color buffer.
            GLES20.glGenTextures(1, values, 0);
            GlUtil.checkGlError("glGenTextures");
            mOffscreenTexture = values[0];   // expected > 0
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, mOffscreenTexture);
            GlUtil.checkGlError("glBindTexture " + mOffscreenTexture);

            // Create texture storage.
            GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_RGBA, width, height, 0,
                    GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE, null);

            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_CLAMP_TO_EDGE);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_CLAMP_TO_EDGE);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR);

            GlUtil.checkGlError("glTexParameter");

            // Create framebuffer object and bind it.
            GLES20.glGenFramebuffers(1, values, 0);
            GlUtil.checkGlError("glGenFramebuffers");
            mFramebuffer = values[0];    // expected > 0
            GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, mFramebuffer);
            GlUtil.checkGlError("glBindFramebuffer " + mFramebuffer);

            // Create a depth buffer and bind it.
            GLES20.glGenRenderbuffers(1, values, 0);
            GlUtil.checkGlError("glGenRenderbuffers");
            mDepthBuffer = values[0];    // expected > 0
            GLES20.glBindRenderbuffer(GLES20.GL_RENDERBUFFER, mDepthBuffer);
            GlUtil.checkGlError("glBindRenderbuffer " + mDepthBuffer);

            // Allocate storage for the depth buffer.
            GLES20.glRenderbufferStorage(GLES20.GL_RENDERBUFFER, GLES20.GL_DEPTH_COMPONENT16,
                    width, height);
            GlUtil.checkGlError("glRenderbufferStorage");

            // Attach the depth buffer and the texture (color buffer) to the framebuffer object.
            GLES20.glFramebufferRenderbuffer(GLES20.GL_FRAMEBUFFER, GLES20.GL_DEPTH_ATTACHMENT,
                    GLES20.GL_RENDERBUFFER, mDepthBuffer);
            GlUtil.checkGlError("glFramebufferRenderbuffer");
            GLES20.glFramebufferTexture2D(GLES20.GL_FRAMEBUFFER, GLES20.GL_COLOR_ATTACHMENT0,
                    GLES20.GL_TEXTURE_2D, mOffscreenTexture, 0);
            GlUtil.checkGlError("glFramebufferTexture2D");

            // See if GLES is happy with all this.
            int status = GLES20.glCheckFramebufferStatus(GLES20.GL_FRAMEBUFFER);
            if (status != GLES20.GL_FRAMEBUFFER_COMPLETE) {
//                throw new RuntimeException("Framebuffer not complete, status=" + status);
            }

            // Switch back to the default framebuffer.
            GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);

            GlUtil.checkGlError("prepareFramebuffer done");
            mWidth = width;
            mHeight = height;
        }
    }


}
