package com.app.jagles.HWCodec;

import android.opengl.EGL14;
import android.opengl.GLES20;
import android.opengl.GLES30;
import android.opengl.Matrix;
import android.util.Log;
import android.view.SurfaceHolder;

import com.app.jagles.HWCodec.gles.EglCore;
import com.app.jagles.HWCodec.gles.WindowSurface;
import com.app.jagles.video.GLVideoDisplay;

import java.io.File;
import java.io.IOException;
import java.nio.ByteBuffer;

import android.opengl.EGL14;
import android.opengl.EGLConfig;
import android.opengl.EGLContext;
import android.opengl.EGLDisplay;
import android.opengl.EGLExt;
import android.opengl.EGLSurface;

/**
 * Created by hongli on 2017/4/26.
 */

public class HWMP4Encoder {

    private static final String TAG = "HWMP4Encoder";
    public boolean mRecordingEnabled = false;
    private File mOutputFile = null;
    private WindowSurface mInputWindowSurface = null;
    private TextureMovieEncoder2 mVideoEncoder = null;
    private GLVideoDisplay mView = null;
    private EglCore mEglCore = null;
    private EGLDisplay mDisplay = null;
    private EGLContext mContext = null;
    private EGLSurface mSurface = null;
    private FullFrameRect mFullScreen = null;
    private VideoEncoderCore encoderCore;
    private final float[] mIdentityMatrix;
    public int mWidth = 0;
    public int mHeight = 0;
    private long mTimeStampNanos = 0;

    private ByteBuffer mAudiobuffer = ByteBuffer.allocate(2048);
    private int mAudioPosition = 0;


    public HWMP4Encoder(GLVideoDisplay view) {
        mView = view;
        mDisplay = EGL14.eglGetDisplay(EGL14.EGL_DEFAULT_DISPLAY);
        mContext = EGL14.eglGetCurrentContext();
        mSurface = EGL14.eglGetCurrentSurface(EGL14.EGL_READ);
        mIdentityMatrix = new float[16];
        Matrix.setIdentityM(mIdentityMatrix, 0);
    }

    public void StartRecord(int w, int h, String Filename, EglCore core) {

        mEglCore = core;
        h = (int) ((h / 16) + 1) * 16;
        w = (int) ((w / 16) + 1) * 16;
        mOutputFile = new File(Filename);
        try {
            Log.d(TAG, "StartRecord: ----->" + h + "-----" + w);
            encoderCore = new VideoEncoderCore(w, h,
                    2048000, mOutputFile);
        } catch (IOException ioe) {
//            Log.d("zasko", "StartRecord: ------------>��ʼ¼���쳣");
            throw new RuntimeException(ioe);
        }
        mWidth = w;
        mHeight = h;
        mInputWindowSurface = new WindowSurface(mEglCore, encoderCore.getInputSurface(), true);
        mVideoEncoder = new TextureMovieEncoder2(encoderCore);
        mTimeStampNanos = System.nanoTime();
        mRecordingEnabled = true;
        mInputWindowSurface.makeCurrent();
        mFullScreen = new FullFrameRect(
                new Texture2dProgram(Texture2dProgram.ProgramType.TEXTURE_2D));
        encoderCore.startRecording();
        //EGL14.eglMakeCurrent(mDisplay,mSurface,mSurface,mContext);
    }

    public void WriteFrame(int texid) {
        if (mInputWindowSurface == null)
            return;
        mVideoEncoder.frameAvailableSoon();
        mInputWindowSurface.makeCurrent();
        GLES20.glClearColor(0f, 0f, 0f, 1f);
//        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);
//        GLES20.glDisable(GLES20.GL_SCISSOR_TEST);
//        GLES20.glViewport(0,0,mWidth,mHeight);
//        mFullScreen.drawFrame(texid,mIdentityMatrix);
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);
        GLES20.glEnable(GLES20.GL_BLEND);
        GLES20.glBlendFunc(GLES20.GL_SRC_ALPHA, GLES20.GL_ONE_MINUS_SRC_ALPHA);
        GLES20.glViewport(0, 0, mWidth, mHeight);
//        if(mEglCore.getGlVersion()>=3)
//        {
//            GLES30.glBlitFramebuffer(0,0,mWidth,mHeight,0,0,mWidth,mHeight,GLES30.GL_COLOR_BUFFER_BIT,GLES30.GL_NEAREST);
//        }
//        else {
        mView.mRender.DrawParametric(mView.mRender.mParametricManager);
//        }
        mInputWindowSurface.setPresentationTime(mTimeStampNanos);
        mInputWindowSurface.swapBuffers();
        mTimeStampNanos = System.nanoTime();

        //EGL14.eglMakeCurrent(mDisplay,mSurface,mSurface,mContext);
        Log.d("honglee_old", "writeframe.................................");
    }

    public void StopRecord() {
        encoderCore.stopRecording();
        if (mVideoEncoder != null) {
            Log.d("honglee_old", "stopping recorder, mVideoEncoder=" + mVideoEncoder);
            mVideoEncoder.stopRecording();
            // TODO: wait (briefly) until it finishes shutting down so we know file is
            //       complete, or have a callback that updates the UI
            mVideoEncoder = null;
        }
        if (mInputWindowSurface != null) {
            mInputWindowSurface.release();
            mInputWindowSurface = null;
        }
        mRecordingEnabled = false;
        if(mEglCore!=null)
        {
            //mEglCore.release();
        }
    }

    public void writeAudioData(byte[] buffer) {

//        Log.d(TAG, "writeAudioData: ---->" + encoderCore.hashCode() + "---" + buffer.length);
        if (encoderCore != null) {
            if (buffer.length > 2048) {
                for (int i = 0; i < buffer.length / 2048; i++) {
                    mAudiobuffer.position(0);
                    mAudiobuffer.put(buffer, i * 2048, 2048);
                    encoderCore.sendAudioToEncoder(mAudiobuffer.array());
                    encoderCore.P_drainEncoder(false, false);
                }
                if (buffer.length > (buffer.length / 2048) * 2048) {
                    int count = buffer.length / 2048;
                    mAudiobuffer.put(buffer, (count - 1) * 2048, buffer.length - (count - 1) * 2048);
                    mAudioPosition = buffer.length - (count - 1) * 2048;
                    int len = buffer.length;
                    //Log.d("honglee_0828","size--->"+len+","+(len-(count-1)*2048));
                }
            } else if (buffer.length + mAudioPosition > 2048) {
                mAudiobuffer.put(buffer, 0, 2048 - mAudioPosition);
                encoderCore.sendAudioToEncoder(mAudiobuffer.array());
                encoderCore.P_drainEncoder(false, false);
                mAudiobuffer.position(0);
                mAudiobuffer.put(buffer, 2048 - mAudioPosition, buffer.length + mAudioPosition - 2048);
                mAudioPosition = buffer.length + mAudioPosition - 2048;
            } else {
                mAudiobuffer.put(buffer);
                mAudioPosition = mAudioPosition + buffer.length;
            }
        }
    }
}
