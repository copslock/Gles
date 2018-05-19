package com.app.jagles.HWCodec;

import android.graphics.SurfaceTexture;
import android.media.MediaCodec;
import android.opengl.GLES20;
import android.opengl.Matrix;
import android.util.Log;
import android.view.Surface;

import com.app.jagles.HWCodec.gles.MoviePlayer;
import com.app.jagles.HWCodec.gles.SpeedControlCallback;
import com.app.jagles.video.GLVideoDisplay;

import java.io.File;
import java.io.IOException;
import java.nio.ByteBuffer;

import static android.opengl.GLES11Ext.GL_TEXTURE_EXTERNAL_OES;
import static android.opengl.GLES20.GL_CULL_FACE;
import static android.opengl.GLES20.GL_SCISSOR_TEST;

/**
 * Created by hongli on 2017/5/6.
 */

public class HWPlayer implements SurfaceTexture.OnFrameAvailableListener,MoviePlayer.PlayerFeedback,MoviePlayer.TimeStamp,IFrameCallback {
    public final String TAG="honglee";
    public int mOffscreenTexture=0;
    private int mFramebuffer;
    private int mDepthBuffer;
    public int mWidth=0;
    public int mHeight=0;
    public long mFileDurationTime = 0;
    private int mDecodeTextureID=0;
    private SurfaceTexture mSurfacetex;
    private Surface mSurface;
    private FullFrameRect mFullScreen;
    private GLVideoDisplay mView;
    private final float[] mIdentityMatrix;
    private static final boolean VERBOSE = false;
    private int mUpdateSurface = 0;
    public boolean isSupportHardWare;
    private ByteBuffer sps = null;
    private ByteBuffer pps = null;
    private long mTimestamp = 0;
    public boolean mHaveImage = false;
    //    private MoviePlayer.PlayTask mPlayTask;
    public boolean mIsPlaying = false;
    private PresentTimeCallback mCallback = null;
    private PresentAudioData mAudioCallback = null;

    private MediaMoviePlayer mPlayer1 = null;

    //IFrameCallback

    @Override
    public void onPrepared() {
        mView.queueEvent(new Runnable() {
            @Override
            public void run() {
                mWidth = mPlayer1.getWidth();
                mHeight = mPlayer1.getHeight();
                mFileDurationTime = mPlayer1.getDurationUs();
                prepareFramebuffer(mWidth, mHeight);
                mPlayer1.play();
                mIsPlaying = true;
            }
        });
    }

    /**
     * called when playing finished
     */
    @Override
    public void onFinished()
    {
        if(mPlayer1 != null)
        {
            mPlayer1.stop();
            mPlayer1 = null;
            mIsPlaying = false;
        }
    }

    /**
     * called every frame before time adjusting
     * return true if you don't want to use internal time adjustment
     */
    private long lasttime = 0;
    private long mAudioTime = 0;

    @Override
    public boolean onFrameAvailable(long presentationTimeUs, boolean audio) {
//        Log.d("honglee_0828","time------>"+presentationTimeUs);
//        if (lasttime > presentationTimeUs) {
//            Log.d(TAG, "onFrameAvailable: --------->---------------->" + lasttime + "--" + presentationTimeUs);
//            return false;
//        }
//        lasttime = presentationTimeUs;
//        if (mCallback != null) {
//            mCallback.PresentTime(presentationTimeUs / 1000L);
//        }
//        return true;
//        if (audio) {
//            return false;
//        }
        Log.d(TAG, "onFrameAvailable: --------->---------------->" + presentationTimeUs + "--" + lasttime);

        if (audio) {
            if (presentationTimeUs > mAudioTime) {
                mAudioTime = presentationTimeUs;

                return true;
            }


            return false;
        } else {
            if (mCallback != null) {
                mCallback.PresentTime(presentationTimeUs / 1000L);
            }
            return false;
        }


    }

    @Override
    public void OnAudioData(byte[] data)
    {
        if(mAudioCallback!=null)
        {
            mAudioCallback.OnAudioData(data);
        }
    }

    public void EnabledAudio(boolean value) {
        if(mPlayer1 != null)
        {
            mPlayer1.EnabledAudio(value);
        }
    }

    public void SetPresentTimeCallback(PresentTimeCallback pts)
    {
        mCallback = pts;
    }
    public void SetPresetnAudioCallback( PresentAudioData callback){ mAudioCallback = callback;}

    public interface PresentTimeCallback {
        public void PresentTime(long ms);
    }

    public interface PresentAudioData {
        public void OnAudioData(byte[] data);
    }

    public HWPlayer(GLVideoDisplay view) {
        mView = view;
        mIdentityMatrix = new float[16];
        Matrix.setIdentityM(mIdentityMatrix, 0);
    }

    public void Release()
    {
        if(mOffscreenTexture!=0)
        {
            int[] a=new int[2];
            a[0] = mOffscreenTexture;
            GLES20.glDeleteTextures(1,a,0);
            mOffscreenTexture = 0;
        }
        if(mDecodeTextureID!=0)
        {
            int[] a=new int[2];
            a[0] = mDecodeTextureID;
            GLES20.glDeleteTextures(1,a,0);
            mDecodeTextureID = 0;
        }
        mWidth = 0;
        mHeight = 0;
        mIsPlaying = false;
    }

    @Override
    public void TimeStamp(long ms)
    {
        if(mCallback!=null)
            mCallback.PresentTime(ms);
    }

    @Override
    synchronized public void onFrameAvailable(SurfaceTexture surface) {
        Log.d("honglee_old", "HWPlayer-------------onFrameAvailable...........");
        mUpdateSurface++;
//        if(!mHaveImage)
//            mView.mRender.DoStatus(mView.mRender.mParametricManager, "   ", 0, 0);
        mHaveImage = true;
    }

    @Override   // MoviePlayer.PlayerFeedback
    public void playbackStopped() {
        Log.d(TAG, "playback stopped");
        //mPlayTask = null;
        Release();
        mIsPlaying = false;
    }

    public boolean GetUpdateSurface()
    {
        synchronized (this) {
            boolean b = mUpdateSurface!=0;
            //if(b)
                mUpdateSurface--;
            return b;
        }
    }


    public boolean SeekToTime(long ms) {
        if (mPlayer1 != null) {
            mPlayer1.seek(ms);
            return true;
        }
        //mPlayTask.SeekToTime(ms);
        return false;
    }

    public void UpdateSurfaceImage()
    {
        synchronized (this) {
            //Log.d("honglee_old", "UpdateSurfaceImage...........");
            mSurfacetex.getTransformMatrix(mIdentityMatrix);
            mSurfacetex.updateTexImage();
        }
    }

    public void MakeCurrent()
    {
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER,mFramebuffer);
    }

    public void ReleaseCurrent()
    {
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER,0);
    }

    public void Draw(boolean getCrop)
    {
        MakeCurrent();
        GLES20.glClearColor(0f,0f,0f,1f);
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);
        GLES20.glDisable(GL_CULL_FACE);
        GLES20.glDisable(GL_SCISSOR_TEST);
        GLES20.glViewport(0,0,mWidth,mHeight);
        mFullScreen.drawFrame(mDecodeTextureID,mIdentityMatrix);
        GLES20.glFlush();
        //mView.RenderBuffer();
        ReleaseCurrent();
    }

    public void PlayFile(String filename)
    {
        SpeedControlCallback callback = new SpeedControlCallback();
        MoviePlayer player = null;
        InitDecoderSurface();
//        try {
//            player = new MoviePlayer(
//                    new File(filename), mSurface, callback);
//        } catch (IOException ioe) {
//            Log.e(TAG, "Unable to play movie", ioe);
//            mSurface.release();
//            return;
//        }
//        player.SetTimeStamp(this);
//        mWidth = player.getVideoWidth();
//        mHeight = player.getVideoHeight();
//        mFileDurationTime = player.getDurationTime();
//        prepareFramebuffer(mWidth,mHeight);
//        mPlayTask = new MoviePlayer.PlayTask(player, this);
//        mPlayTask.execute();
        mPlayer1 = new MediaMoviePlayer(mSurface,this,true);
        mPlayer1.prepare(filename);
    }

    public void Stop()
    {
//        if (mPlayTask != null) {
//            mPlayTask.requestStop();
//            mIsPlaying = false;
//        }
        if(mPlayer1 != null)
        {
            mPlayer1.stop();
            mIsPlaying = false;
        }
    }

    public void Pause()
    {
//        if(mPlayTask != null)
//            mPlayTask.Pause();
        if(mPlayer1 != null)
        {
            mPlayer1.pasuse();
        }
    }

    public void Continue()
    {
//        if(mPlayTask!=null)
//            mPlayTask.Continue();
        if(mPlayer1 != null)
        {
            mPlayer1.resume();
        }
    }

    public void InitDecoderSurface()
    {
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
        mSurface = new Surface(mSurfacetex);
        //GLES20.glViewport(0,0,w,h);
        mFullScreen = new FullFrameRect(
                new Texture2dProgram(Texture2dProgram.ProgramType.TEXTURE_EXT));

    }

    public void prepareFramebuffer(int width, int height) {
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
            throw new RuntimeException("Framebuffer not complete, status=" + status);
        }

        // Switch back to the default framebuffer.
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);

        GlUtil.checkGlError("prepareFramebuffer done");
    }


}
