package com.app.jagles.HWCodec;
/*
 * AudioVideoPlayerSample
 * Sample project to play audio and video from MPEG4 file using MediaCodec.
 *
 * Copyright (c) 2014-2015 saki t_saki@serenegiant.com
 *
 * File name: MediaMoviePlayer.java
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 * All files in the folder are under this Apache License, Version 2.0.
*/

import android.annotation.TargetApi;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.media.MediaCodec;
import android.media.MediaExtractor;
import android.media.MediaFormat;
import android.media.MediaMetadataRetriever;
import android.os.Build;
import android.text.TextUtils;
import android.util.Log;
import android.view.Surface;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.nio.ByteBuffer;

public class MediaMoviePlayer {
    private static final boolean DEBUG = true;
    private static final String TAG_STATIC = "MediaMoviePlayer:";
    private final String TAG = TAG_STATIC + getClass().getSimpleName();

    private final IFrameCallback mCallback;
    private final boolean mAudioEnabled;
    private boolean mPlayAudio = false;

    public MediaMoviePlayer(final Surface outputSurface, final IFrameCallback callback, final boolean audio_enable) throws NullPointerException {
        if (DEBUG) Log.v(TAG, "Constructor:");
        if ((outputSurface == null) || (callback == null))
            throw new NullPointerException("outputSurface and callback should not be null");

        mOutputSurface = outputSurface;
        mCallback = callback;
        mAudioEnabled = audio_enable;
        new Thread(mMoviePlayerTask, TAG).start();
        synchronized (mSync) {
            try {
                if (!mIsRunning)
                    mSync.wait();
            } catch (final InterruptedException e) {
            }
        }
    }

    public final int getWidth() {
        return mVideoWidth;
    }

    public final int getHeight() {
        return mVideoHeight;
    }

    public final int getBitRate() {
        return mBitrate;
    }

    public final float getFramerate() {
        return mFrameRate;
    }

    /**
     * @return 0, 90, 180, 270
     */
    public final int getRotation() {
        return mRotation;
    }

    /**
     * get duration time as micro seconds
     *
     * @return
     */
    public final long getDurationUs() {
        return mDuration;
    }

    /**
     * get audio sampling rate[Hz]
     *
     * @return
     */
    public final int getSampleRate() {
        return mAudioSampleRate;
    }

    public final boolean hasAudio() {
        return mHasAudio;
    }

    /**
     * request to prepare movie playing
     *
     * @param src_movie
     */
    public final void prepare(final String src_movie) {
        if (DEBUG) Log.v(TAG, "prepare:");
        synchronized (mSync) {
            mSourcePath = src_movie;
            mRequest = REQ_PREPARE;
            mSync.notifyAll();
            mPreRenderClass = new PreRenderClass();
            mPreRenderClassAudio = new PreRenderClass();
        }
    }

    private PreRenderClass mPreRenderClassAudio;
    private PreRenderClass mPreRenderClass;

    public void EnabledAudio(boolean value) {
        synchronized (mSync) {
            Log.d(TAG, "EnabledAudio: ------->" + value);
            mPlayAudio = value;
        }
    }

    /**
     * request to start playing movie
     * this method can be called after prepare
     */
    public final void play() {
        if (DEBUG) Log.v(TAG, "play:");
        synchronized (mSync) {
            if (mState == STATE_PLAYING) return;
            mRequest = REQ_START;
            mSync.notifyAll();
        }
    }

    /**
     * request to seek to specifc timed frame<br>
     * if the frame is not a key frame, frame image will be broken
     *
     * @param newTime seek to new time[usec]
     */
    public final void seek(final long newTime) {
        if (DEBUG) Log.v(TAG, "seek");
        synchronized (mSync) {
            mRequest = REQ_SEEK;
            mRequestTime = newTime;
            mSync.notifyAll();
        }
    }

    /**
     * request stop playing
     */
    public final void stop() {
        if (DEBUG) Log.v(TAG, "stop:");
        synchronized (mSync) {
            if (mState != STATE_STOP) {
                mRequest = REQ_STOP;
                mSync.notifyAll();
                try {
                    mSync.wait(50);
                } catch (final InterruptedException e) {
                }
            }
        }
    }

    /**
     * request pause playing<br>
     * this function is un-implemented yet
     */
    public final void pasuse() {
        if (DEBUG) Log.v(TAG, "pasuse:");
        synchronized (mSync) {
            mRequest = REQ_PAUSE;
            mSync.notifyAll();
        }
    }

    /**
     * request resume from pausing<br>
     * this function is un-implemented yet
     */
    public final void resume() {
        if (DEBUG) Log.v(TAG, "resume:");
        synchronized (mSync) {
            mRequest = REQ_RESUME;
            mSync.notifyAll();
        }
    }

    /**
     * release releated resources
     */
    public final void release() {
        if (DEBUG) Log.v(TAG, "release:");
        stop();
        synchronized (mSync) {
            mRequest = REQ_QUIT;
            mSync.notifyAll();
        }
    }

    //================================================================================
    private static final int TIMEOUT_USEC = 1000;    // 10msec

    /*
     * STATE_CLOSED => [preapre] => STATE_PREPARED [start]
     * 	=> STATE_PLAYING => [seek] => STATE_PLAYING
     * 		=> [pause] => STATE_PAUSED => [resume] => STATE_PLAYING
     * 		=> [stop] => STATE_CLOSED
     */
    private static final int STATE_STOP = 0;
    private static final int STATE_PREPARED = 1;
    private static final int STATE_PLAYING = 2;
    private static final int STATE_PAUSED = 3;

    // request code
    private static final int REQ_NON = 0;
    private static final int REQ_PREPARE = 1;
    private static final int REQ_START = 2;
    private static final int REQ_SEEK = 3;
    private static final int REQ_STOP = 4;
    private static final int REQ_PAUSE = 5;
    private static final int REQ_RESUME = 6;
    private static final int REQ_QUIT = 9;

//	private static final long EPS = (long)(1 / 240.0f * 1000000);	// 1/240 seconds[micro seconds]

    protected MediaMetadataRetriever mMetadata;
    private final Object mSync = new Object();
    private volatile boolean mIsRunning;
    private int mState;
    private String mSourcePath;
    private long mDuration;
    private int mRequest;
    private long mRequestTime;
    // for video playback
    private final Object mVideoSync = new Object();
    private final Surface mOutputSurface;
    protected MediaExtractor mVideoMediaExtractor;
    private MediaCodec mVideoMediaCodec;
    private MediaCodec.BufferInfo mVideoBufferInfo;
    private ByteBuffer[] mVideoInputBuffers;
    private ByteBuffer[] mVideoOutputBuffers;
    private long mVideoStartTime;
    @SuppressWarnings("unused")
    private long previousVideoPresentationTimeUs = -1;
    private volatile int mVideoTrackIndex;
    private boolean mVideoInputDone;
    private boolean mVideoOutputDone;
    private int mVideoWidth, mVideoHeight;
    private int mBitrate;
    private float mFrameRate;
    private int mRotation;
    // for audio playback
    private final Object mAudioSync = new Object();
    protected MediaExtractor mAudioMediaExtractor;
    private MediaCodec mAudioMediaCodec;
    private MediaCodec.BufferInfo mAudioBufferInfo;
    private ByteBuffer[] mAudioInputBuffers;
    private ByteBuffer[] mAudioOutputBuffers;
    private long mAudioStartTime;
    @SuppressWarnings("unused")
    private long previousAudioPresentationTimeUs = -1;
    private volatile int mAudioTrackIndex;
    private boolean mAudioInputDone;
    private boolean mAudioOutputDone;
    private int mAudioChannels;
    private int mAudioSampleRate;
    private int mAudioInputBufSize;
    private boolean mHasAudio;
    private byte[] mAudioOutTempBuf;
    private AudioTrack mAudioTrack;

    //实现暂停
    private boolean mPause;

    /**
     * 样本输入的时间戳
     */
    private long mInputSampleTimeUs;
    /**
     * 解码输出时间戳
     */
    private long mLastPresentationTimeUs = -1;
    /**
     * 【在有些手机上】往回定位的时候，解码输出时间会一直与定位前的时间一样，
     * 这会导致播放时间回调不准确，且FPS同步也不生效直到解码输出时间大于定位前
     * 的时间，mLastStopTimeUs用来记录定位前的解码输出时间戳，比较播放进度是否
     * 已经大于定位前的时间，如：
     * 一段10s的视频，播放到7705ms（指解码输出时间戳）的时候，拖拽定位到2000ms，
     * 这时解码输出时间戳一直是7705ms，直到播放的进度大于7705ms，如8000ms，这时
     * 解码输出时间戳才会输出时间播放的时间戳
     */
    private long mLastStopTimeUs = -1;

    /**
     * 样本定位时间戳
     */
    private long mLastOffset = 0;
    private long mLastAudioOffset = 0;

    //是否有定位操作
    private boolean mIsSought;
    private boolean mIsAudioSought;

    //--------------------------------------------------------------------------------
    /**
     * playback control task
     */
    private final Runnable mMoviePlayerTask = new Runnable() {
        @Override
        public final void run() {
            boolean local_isRunning = false;
            int local_req;
            try {
                synchronized (mSync) {
                    local_isRunning = mIsRunning = true;
                    mState = STATE_STOP;
                    mRequest = REQ_NON;
                    mRequestTime = -1;
                    mSync.notifyAll();
                }
                for (; local_isRunning; ) {
                    try {
                        synchronized (mSync) {
                            local_isRunning = mIsRunning;
                            local_req = mRequest;
                            mRequest = REQ_NON;
                        }
                        switch (mState) {
                            case STATE_STOP:
                                local_isRunning = processStop(local_req);
                                break;
                            case STATE_PREPARED:
                                local_isRunning = processPrepared(local_req);
                                break;
                            case STATE_PLAYING:
                                local_isRunning = processPlaying(local_req);
                                break;
                            case STATE_PAUSED:
                                local_isRunning = processPaused(local_req);
                                break;
                        }
                    } catch (final InterruptedException e) {
                        break;
                    } catch (final Exception e) {
                        Log.e(TAG, "MoviePlayerTask:", e);
                        break;
                    }
                } // for (;local_isRunning;)
            } finally {
                if (DEBUG) Log.v(TAG, "player task finished:local_isRunning=" + local_isRunning);
                handleStop();
            }
        }
    };

//--------------------------------------------------------------------------------
    /**
     * video playback task
     */
    private final Runnable mVideoTask = new Runnable() {
        @Override
        public void run() {
            if (DEBUG) Log.v(TAG, "VideoTask:start");
            for (; mIsRunning && !mVideoInputDone && !mVideoOutputDone; ) {
                try {
                    if (mPause) {
                        try {
                            Thread.sleep(10);
                        } catch (Exception e) {
                            e.printStackTrace();
                        }
                        continue;
                    }
                    if (!mVideoInputDone) {
                        handleInputVideo();
                    }
                    if (!mVideoOutputDone) {
                        handleOutputVideo(mCallback);
                    }
                } catch (final Exception e) {
                    Log.e(TAG, "VideoTask:", e);
                    break;
                }
            } // end of for
            if (DEBUG) Log.v(TAG, "VideoTask:finished");
            synchronized (mSync) {
                mVideoInputDone = mVideoOutputDone = true;
                mSync.notifyAll();
            }
        }
    };

//--------------------------------------------------------------------------------
    /**
     * audio playback task
     */
    private final Runnable mAudioTask = new Runnable() {
        @Override
        public void run() {
            if (DEBUG) Log.v(TAG, "AudioTask:start");
            for (; mIsRunning && !mAudioInputDone && !mAudioOutputDone; ) {
                try {
                    if (mPause) {
                        //Log.d(TAG, "run: ----------------");
                        try {
                            Thread.sleep(10);
                        } catch (Exception e) {
                            e.printStackTrace();
                        }
                        continue;
                    }
                    if (!mAudioInputDone) {
                        handleInputAudio();
                    }
                    if (!mAudioOutputDone) {
                        handleOutputAudio(mCallback);
                    }
                } catch (final Exception e) {
                    Log.e(TAG, "VideoTask:", e);
                    break;
                }
            } // end of for
            if (DEBUG) Log.v(TAG, "AudioTask:finished");
            synchronized (mSync) {
                mAudioInputDone = mAudioOutputDone = true;
                mSync.notifyAll();
            }
        }
    };

//--------------------------------------------------------------------------------

    /**
     * @param req
     * @return
     * @throws InterruptedException
     * @throws IOException
     */
    private final boolean processStop(final int req) throws InterruptedException, IOException {
        boolean local_isRunning = true;
        switch (req) {
            case REQ_PREPARE:
                handlePrepare(mSourcePath);
                break;
            case REQ_START:
            case REQ_PAUSE:
            case REQ_RESUME:
                throw new IllegalStateException("invalid state:" + mState);
            case REQ_QUIT:
                local_isRunning = false;
                break;
//		case REQ_SEEK:
//		case REQ_STOP:
            default:
                synchronized (mSync) {
                    mSync.wait();
                }
                break;
        }
        synchronized (mSync) {
            local_isRunning &= mIsRunning;
        }
        return local_isRunning;
    }

    /**
     * @param req
     * @return
     * @throws InterruptedException
     */
    private final boolean processPrepared(final int req) throws InterruptedException {
        boolean local_isRunning = true;
        switch (req) {
            case REQ_START:
                handleStart();
                break;
            case REQ_PAUSE:
            case REQ_RESUME:
                throw new IllegalStateException("invalid state:" + mState);
            case REQ_STOP:
                handleStop();
                break;
            case REQ_QUIT:
                local_isRunning = false;
                break;
//		case REQ_PREPARE:
//		case REQ_SEEK:
            default:
                synchronized (mSync) {
                    mSync.wait();
                }
                break;
        } // end of switch (req)
        synchronized (mSync) {
            local_isRunning &= mIsRunning;
        }
        return local_isRunning;
    }

    /**
     * @param req
     * @return
     */
    private final boolean processPlaying(final int req) {
        boolean local_isRunning = true;
        switch (req) {
            case REQ_PREPARE:
            case REQ_START:
            case REQ_RESUME:
                throw new IllegalStateException("invalid state:" + mState);
            case REQ_SEEK:
                handleSeek(mRequestTime);
                break;
            case REQ_STOP:
                handleStop();
                break;
            case REQ_PAUSE:
                handlePause();
                break;
            case REQ_QUIT:
                local_isRunning = false;
                break;
            default:
                handleLoop(mCallback);
                break;
        } // end of switch (req)
        synchronized (mSync) {
            local_isRunning &= mIsRunning;
        }
        return local_isRunning;
    }

    /**
     * @param req
     * @return
     * @throws InterruptedException
     */
    private final boolean processPaused(final int req) throws InterruptedException {
        boolean local_isRunning = true;
        switch (req) {
            case REQ_PREPARE:
            case REQ_START:
                throw new IllegalStateException("invalid state:" + mState);
            case REQ_SEEK:
                handleSeek(mRequestTime);
                break;
            case REQ_STOP:
                handleStop();
                break;
            case REQ_RESUME:
                handleResume();
                break;
            case REQ_QUIT:
                local_isRunning = false;
                break;
//		case REQ_PAUSE:
            default:
                synchronized (mSync) {
                    mSync.wait();
                }
                break;
        } // end of switch (req)
        synchronized (mSync) {
            local_isRunning &= mIsRunning;
        }
        return local_isRunning;
    }

//--------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------

    /**
     * @param source_file
     * @throws IOException
     */
    private final void handlePrepare(final String source_file) throws IOException {
        if (DEBUG) Log.v(TAG, "handlePrepare:" + source_file);
        synchronized (mSync) {
            if (mState != STATE_STOP) {
                throw new RuntimeException("invalid state:" + mState);
            }
        }
        //mPrevMonoUsec = 0;
        mPreRenderClassAudio.Reset();
        mPreRenderClass.Reset();
        final File src = new File(source_file);
        if (TextUtils.isEmpty(source_file) || !src.canRead()) {
            throw new FileNotFoundException("Unable to read " + source_file);
        }
        mVideoTrackIndex = mAudioTrackIndex = -1;
        mMetadata = new MediaMetadataRetriever();
        mMetadata.setDataSource(source_file);
        updateMovieInfo();
        // preparation for video playback
        mVideoTrackIndex = internal_prepare_video(source_file);
        // preparation for audio playback
        if (mAudioEnabled)
            mAudioTrackIndex = internal_prepare_audio(source_file);
        mHasAudio = mAudioTrackIndex >= 0;
        if ((mVideoTrackIndex < 0) && (mAudioTrackIndex < 0)) {
            throw new RuntimeException("No video and audio track found in " + source_file);
        }
        synchronized (mSync) {
            mState = STATE_PREPARED;
        }
        mCallback.onPrepared();
    }

    /**
     * @return first video track index, -1 if not found
     */
    @TargetApi(Build.VERSION_CODES.JELLY_BEAN)
    protected int internal_prepare_video(final String source_path) {
        int trackindex = -1;
        mVideoMediaExtractor = new MediaExtractor();
        try {
            mVideoMediaExtractor.setDataSource(source_path);
            trackindex = selectTrack(mVideoMediaExtractor, "video/");
            if (trackindex >= 0) {
                mVideoMediaExtractor.selectTrack(trackindex);
                final MediaFormat format = mVideoMediaExtractor.getTrackFormat(trackindex);
                mVideoWidth = format.getInteger(MediaFormat.KEY_WIDTH);
                mVideoHeight = format.getInteger(MediaFormat.KEY_HEIGHT);
                mDuration = format.getLong(MediaFormat.KEY_DURATION);

                if (DEBUG)
                    Log.v(TAG, String.format("format:size(%d,%d),duration=%d,bps=%d,framerate=%f,rotation=%d",
                            mVideoWidth, mVideoHeight, mDuration, mBitrate, mFrameRate, mRotation));
            }
        } catch (final IOException e) {
        }
        return trackindex;
    }

    /**
     * @return first audio track index, -1 if not found
     */
    protected int internal_prepare_audio(final String source_file) {
        int trackindex = -1;
        mAudioMediaExtractor = new MediaExtractor();
        try {
            mAudioMediaExtractor.setDataSource(source_file);
            trackindex = selectTrack(mAudioMediaExtractor, "audio/");
            if (trackindex >= 0) {
                mAudioMediaExtractor.selectTrack(trackindex);
                final MediaFormat format = mAudioMediaExtractor.getTrackFormat(trackindex);
                mAudioChannels = format.getInteger(MediaFormat.KEY_CHANNEL_COUNT);
                mAudioSampleRate = format.getInteger(MediaFormat.KEY_SAMPLE_RATE);
                final int min_buf_size = AudioTrack.getMinBufferSize(mAudioSampleRate,
                        (mAudioChannels == 1 ? AudioFormat.CHANNEL_OUT_MONO : AudioFormat.CHANNEL_OUT_STEREO),
                        AudioFormat.ENCODING_PCM_16BIT);
                final int max_input_size = format.getInteger(MediaFormat.KEY_MAX_INPUT_SIZE);
                mAudioInputBufSize = min_buf_size > 0 ? min_buf_size * 4 : max_input_size;
                if (mAudioInputBufSize > max_input_size) mAudioInputBufSize = max_input_size;
                final int frameSizeInBytes = mAudioChannels * 2;
                mAudioInputBufSize = (mAudioInputBufSize / frameSizeInBytes) * frameSizeInBytes;
                if (DEBUG)
                    Log.v(TAG, String.format("getMinBufferSize=%d,max_input_size=%d,mAudioInputBufSize=%d", min_buf_size, max_input_size, mAudioInputBufSize));
                //
                mAudioTrack = new AudioTrack(AudioManager.STREAM_MUSIC,
                        mAudioSampleRate,
                        (mAudioChannels == 1 ? AudioFormat.CHANNEL_OUT_MONO : AudioFormat.CHANNEL_OUT_STEREO),
                        AudioFormat.ENCODING_PCM_16BIT,
                        mAudioInputBufSize,
                        AudioTrack.MODE_STREAM);
                try {
                    mAudioTrack.play();
                } catch (final Exception e) {
                    Log.e(TAG, "failed to start audio track playing", e);
                    mAudioTrack.release();
                    mAudioTrack = null;
                }
            }
        } catch (final IOException e) {
        }
        return trackindex;
    }

    protected void updateMovieInfo() {
        mVideoWidth = mVideoHeight = mRotation = mBitrate = 0;
        mDuration = 0;
        mFrameRate = 0;
        String value = mMetadata.extractMetadata(MediaMetadataRetriever.METADATA_KEY_VIDEO_WIDTH);
        if (!TextUtils.isEmpty(value)) {
            mVideoWidth = Integer.parseInt(value);
        }
        value = mMetadata.extractMetadata(MediaMetadataRetriever.METADATA_KEY_VIDEO_HEIGHT);
        if (!TextUtils.isEmpty(value)) {
            mVideoHeight = Integer.parseInt(value);
        }
        value = mMetadata.extractMetadata(MediaMetadataRetriever.METADATA_KEY_VIDEO_ROTATION);
        if (!TextUtils.isEmpty(value)) {
            mRotation = Integer.parseInt(value);
        }
        value = mMetadata.extractMetadata(MediaMetadataRetriever.METADATA_KEY_BITRATE);
        if (!TextUtils.isEmpty(value)) {
            mBitrate = Integer.parseInt(value);
        }
        value = mMetadata.extractMetadata(MediaMetadataRetriever.METADATA_KEY_DURATION);
        if (!TextUtils.isEmpty(value)) {
            mDuration = Long.parseLong(value) * 1000;
        }
    }

    private final void handleStart() {
        if (DEBUG) Log.v(TAG, "handleStart:");
        synchronized (mSync) {
            if (mState != STATE_PREPARED)
                throw new RuntimeException("invalid state:" + mState);
            mState = STATE_PLAYING;
        }
        if (mRequestTime > 0) {
            handleSeek(mRequestTime);
        }
        previousVideoPresentationTimeUs = previousAudioPresentationTimeUs = -1;
        mVideoInputDone = mVideoOutputDone = true;
        Thread videoThread = null, audioThread = null;
        if (mVideoTrackIndex >= 0) {
            final MediaCodec codec = internal_start_video(mVideoMediaExtractor, mVideoTrackIndex);
            if (codec != null) {
                mVideoMediaCodec = codec;
                mVideoBufferInfo = new MediaCodec.BufferInfo();
                mVideoInputBuffers = codec.getInputBuffers();
                mVideoOutputBuffers = codec.getOutputBuffers();
            }
            mVideoInputDone = mVideoOutputDone = false;
            videoThread = new Thread(mVideoTask, "VideoTask");
        }
        mAudioInputDone = mAudioOutputDone = true;
        if (mAudioTrackIndex >= 0) {
            final MediaCodec codec = internal_start_audio(mAudioMediaExtractor, mAudioTrackIndex);
            if (codec != null) {
                mAudioMediaCodec = codec;
                mAudioBufferInfo = new MediaCodec.BufferInfo();
                mAudioInputBuffers = codec.getInputBuffers();
                mAudioOutputBuffers = codec.getOutputBuffers();
            }
            mAudioInputDone = mAudioOutputDone = false;
            audioThread = new Thread(mAudioTask, "AudioTask");
        }
        if (videoThread != null) videoThread.start();
        if (audioThread != null) audioThread.start();
    }

    /**
     * @param media_extractor
     * @param trackIndex
     * @return
     */
    protected MediaCodec internal_start_video(final MediaExtractor media_extractor, final int trackIndex) {
        if (DEBUG) Log.v(TAG, "internal_start_video:");
        MediaCodec codec = null;
        if (trackIndex >= 0) {
            final MediaFormat format = media_extractor.getTrackFormat(trackIndex);
            final String mime = format.getString(MediaFormat.KEY_MIME);
            try {
                codec = MediaCodec.createDecoderByType(mime);
                codec.configure(format, mOutputSurface, null, 0);
                codec.start();
                if (DEBUG) Log.v(TAG, "internal_start_video:codec started");
            } catch (IOException e) {
                e.printStackTrace();
                codec = null;
            }
        }
        return codec;
    }

    /**
     * @param media_extractor
     * @param trackIndex
     * @return
     */
    protected MediaCodec internal_start_audio(final MediaExtractor media_extractor, final int trackIndex) {
        if (DEBUG) Log.v(TAG, "internal_start_audio:");
        MediaCodec codec = null;
        if (trackIndex >= 0) {
            final MediaFormat format = media_extractor.getTrackFormat(trackIndex);
            final String mime = format.getString(MediaFormat.KEY_MIME);
            try {
                codec = MediaCodec.createDecoderByType(mime);
                codec.configure(format, null, null, 0);
                codec.start();
                if (DEBUG) Log.v(TAG, "internal_start_audio:codec started");
                //
                final ByteBuffer[] buffers = codec.getOutputBuffers();
                int sz = buffers[0].capacity();
                if (sz <= 0)
                    sz = mAudioInputBufSize;
                if (DEBUG) Log.v(TAG, "AudioOutputBufSize:" + sz);
                mAudioOutTempBuf = new byte[sz];
            } catch (IOException e) {
                e.printStackTrace();
                codec = null;
            }
        }
        return codec;
    }

    private final void handleSeek(final long newTime) {
        if (DEBUG) Log.d(TAG, "handleSeek");
        if (newTime < 0) return;


        if (mVideoTrackIndex >= 0) {
            mIsSought = true;
            mVideoMediaExtractor.seekTo(newTime, MediaExtractor.SEEK_TO_CLOSEST_SYNC);
            //mVideoMediaExtractor.advance();

            mLastOffset = mVideoMediaExtractor.getSampleTime() / 1000;
        }
        if (mAudioTrackIndex >= 0) {
            mIsAudioSought = true;
            mAudioMediaExtractor.seekTo(newTime, MediaExtractor.SEEK_TO_CLOSEST_SYNC);
            mLastAudioOffset = mAudioMediaExtractor.getSampleTime() / 1000;
            //mAudioMediaExtractor.advance();
        }
        mRequestTime = -1;
    }

    private final void handleLoop(final IFrameCallback frameCallback) {
//		if (DEBUG) Log.d(TAG, "handleLoop");

        synchronized (mSync) {
            try {
                mSync.wait();
            } catch (final InterruptedException e) {
            }
        }
        if (mVideoInputDone && mVideoOutputDone && mAudioInputDone && mAudioOutputDone) {
            if (DEBUG) Log.d(TAG, "Reached EOS, looping check");
            handleStop();
        }
    }

    /**
     * @param codec
     * @param extractor
     * @param inputBuffers
     * @param presentationTimeUs
     * @param isAudio
     */
    protected boolean internal_process_input(final MediaCodec codec, final MediaExtractor extractor, final ByteBuffer[] inputBuffers, final long presentationTimeUs, final boolean isAudio) {
//		if (DEBUG) Log.v(TAG, "internal_process_input:presentationTimeUs=" + presentationTimeUs);
        boolean result = true;
        while (mIsRunning) {
            final int inputBufIndex = codec.dequeueInputBuffer(TIMEOUT_USEC);
            if (inputBufIndex == MediaCodec.INFO_TRY_AGAIN_LATER)
                break;
            if (inputBufIndex >= 0) {
                final int size = extractor.readSampleData(inputBuffers[inputBufIndex], 0);
                if (size > 0) {
                    codec.queueInputBuffer(inputBufIndex, 0, size, presentationTimeUs, 0);
                    result = extractor.advance();    // return false if no data is available
                } else {
                    codec.queueInputBuffer(inputBufIndex, 0, 0, 0, MediaCodec.BUFFER_FLAG_END_OF_STREAM);
                }

                break;
            }
        }
        return result;
    }

    private final void handleInputVideo() {
        mInputSampleTimeUs = mVideoMediaExtractor.getSampleTime();
        //final long presentationTimeUs = mVideoMediaExtractor.getSampleTime();
/*		if (presentationTimeUs < previousVideoPresentationTimeUs) {
            presentationTimeUs += previousVideoPresentationTimeUs - presentationTimeUs; // + EPS;
    	}
    	previousVideoPresentationTimeUs = presentationTimeUs; */

        final boolean b = internal_process_input(mVideoMediaCodec, mVideoMediaExtractor, mVideoInputBuffers,
                mInputSampleTimeUs, false);
        if (!b) {
            if (DEBUG) Log.i(TAG, "video track input reached EOS");
            while (mIsRunning) {
                final int inputBufIndex = mVideoMediaCodec.dequeueInputBuffer(TIMEOUT_USEC);
                if (inputBufIndex >= 0) {
                    mVideoMediaCodec.queueInputBuffer(inputBufIndex, 0, 0, 0L,
                            MediaCodec.BUFFER_FLAG_END_OF_STREAM);
                    if (DEBUG) Log.v(TAG, "sent input EOS:" + mVideoMediaCodec);
                    break;
                }
            }
            synchronized (mSync) {
                mVideoInputDone = true;
                mSync.notifyAll();
            }
        }
    }

    /**
     * @param frameCallback
     */
    private final void handleOutputVideo(final IFrameCallback frameCallback) {
        //    	if (DEBUG) Log.v(TAG, "handleDrainVideo:");
        int count = 0;
        while (mIsRunning && !mVideoOutputDone) {
            final int decoderStatus = mVideoMediaCodec.dequeueOutputBuffer(mVideoBufferInfo, TIMEOUT_USEC);
            Log.d(TAG, "handleOutputVideo:" + decoderStatus);

//            if (mVideoBufferInfo.presentationTimeUs < mLastPresentationTimeUs) {      // correct timing playback issue for some videos
//                mStartMs = System.currentTimeMillis();
//            }


            if (decoderStatus == MediaCodec.INFO_TRY_AGAIN_LATER) {
                return;
            } else if (decoderStatus == MediaCodec.INFO_OUTPUT_BUFFERS_CHANGED) {
                mVideoOutputBuffers = mVideoMediaCodec.getOutputBuffers();
                if (DEBUG) Log.d(TAG, "INFO_OUTPUT_BUFFERS_CHANGED:");
            } else if (decoderStatus == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
                final MediaFormat newFormat = mVideoMediaCodec.getOutputFormat();
                if (DEBUG) Log.d(TAG, "video decoder output format changed: " + newFormat);
            } else if (decoderStatus < 0) {
                throw new RuntimeException(
                        "unexpected result from video decoder.dequeueOutputBuffer: " + decoderStatus);
            } else { // decoderStatus >= 0

//                if (mLastPresentationTimeUs == mVideoBufferInfo.presentationTimeUs && mLastPresentationTimeUs > 0) {
//                    mLastStopTimeUs = mVideoBufferInfo.presentationTimeUs;
//                    mLastPresentationTimeUs = mInputSampleTimeUs;
//                } else {
//                    if (mLastStopTimeUs != -1) {
//                        if (mLastStopTimeUs != mVideoBufferInfo.presentationTimeUs) {
//                            mLastStopTimeUs = -1;
//                            mLastPresentationTimeUs = mVideoBufferInfo.presentationTimeUs;
//                        } else {
//                            mLastPresentationTimeUs = mInputSampleTimeUs;
//                        }
//                    } else {
//                        mLastPresentationTimeUs = mVideoBufferInfo.presentationTimeUs;
//                    }
//                }
                mLastPresentationTimeUs = mInputSampleTimeUs;

                if (mIsSought) {
                    long diff = Math.abs(mLastPresentationTimeUs / 1000 - mLastOffset);
                    Log.d(TAG, "hqh: mInputSampleTimeUs = " + mInputSampleTimeUs + ", mLastPresentationTimeUs = " + mLastPresentationTimeUs + ", mLastOffset = " + mLastOffset + ", diff = " + diff);
                    if (diff > 50 && diff < 300) {
                        mIsSought = false;
                    }
                }

                boolean doRender = false;
                if (mVideoBufferInfo.size > 0) {
                    doRender = true;
                    //                            && !internal_write_video(mVideoOutputBuffers[decoderStatus],
                    //                            0, mVideoBufferInfo.size, mVideoBufferInfo.presentationTimeUs);
                    frameCallback.onFrameAvailable(mLastPresentationTimeUs, false);
                    //mVideoStartTime = adjustPresentationTime(mVideoSync, mVideoStartTime, mVideoBufferInfo.presentationTimeUs);
                }

                mVideoMediaCodec.releaseOutputBuffer(decoderStatus, doRender);
                if (!mIsSought) {
                    mPreRenderClass.preRender(mLastPresentationTimeUs);
                }
                if ((mVideoBufferInfo.flags & MediaCodec.BUFFER_FLAG_END_OF_STREAM) != 0) {
                    if (DEBUG) Log.d(TAG, "video:output EOS");
                    synchronized (mSync) {
                        mVideoOutputDone = true;
                        mSync.notifyAll();
                    }
                }
            }
        }
    }

    /**
     * @param buffer
     * @param offset
     * @param size
     * @param presentationTimeUs
     */
    protected boolean internal_write_video(final ByteBuffer buffer, final int offset, final int size, final long presentationTimeUs) {
//		if (DEBUG) Log.v(TAG, "internal_write_video");
        return false;
    }

    private final void handleInputAudio() {
        final long presentationTimeUs = mAudioMediaExtractor.getSampleTime();
/*		if (presentationTimeUs < previousAudioPresentationTimeUs) {
            presentationTimeUs += previousAudioPresentationTimeUs - presentationTimeUs; //  + EPS;
    	}
    	previousAudioPresentationTimeUs = presentationTimeUs; */
        final boolean b = internal_process_input(mAudioMediaCodec, mAudioMediaExtractor, mAudioInputBuffers,
                presentationTimeUs, true);
        if (!b) {
            if (DEBUG) Log.i(TAG, "audio track input reached EOS");
            while (mIsRunning) {
                final int inputBufIndex = mAudioMediaCodec.dequeueInputBuffer(TIMEOUT_USEC);
                if (inputBufIndex >= 0) {
                    mAudioMediaCodec.queueInputBuffer(inputBufIndex, 0, 0, 0L,
                            MediaCodec.BUFFER_FLAG_END_OF_STREAM);
                    if (DEBUG) Log.v(TAG, "sent input EOS:" + mAudioMediaCodec);
                    break;
                }
            }
            synchronized (mSync) {
                mAudioInputDone = true;
                mSync.notifyAll();
            }
        }
    }

    private final void handleOutputAudio(final IFrameCallback frameCallback) {
//		if (DEBUG) Log.v(TAG, "handleDrainAudio:");
        while (mIsRunning && !mAudioOutputDone) {
            final int decoderStatus = mAudioMediaCodec.dequeueOutputBuffer(mAudioBufferInfo, TIMEOUT_USEC);
            if (decoderStatus == MediaCodec.INFO_TRY_AGAIN_LATER) {
                return;
            } else if (decoderStatus == MediaCodec.INFO_OUTPUT_BUFFERS_CHANGED) {
                mAudioOutputBuffers = mAudioMediaCodec.getOutputBuffers();
                if (DEBUG) Log.d(TAG, "INFO_OUTPUT_BUFFERS_CHANGED:");
            } else if (decoderStatus == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
                final MediaFormat newFormat = mAudioMediaCodec.getOutputFormat();
                if (DEBUG) Log.d(TAG, "audio decoder output format changed: " + newFormat);
            } else if (decoderStatus < 0) {
                throw new RuntimeException(
                        "unexpected result from audio decoder.dequeueOutputBuffer: " + decoderStatus);
            } else { // decoderStatus >= 0
                if (mAudioBufferInfo.size > 0) {
                    if (mIsAudioSought) {
                        long diff = Math.abs(mAudioBufferInfo.presentationTimeUs / 1000 - mLastAudioOffset);
                        Log.d(TAG, "hqh: handleOutputAudio: mAudioBufferInfo.presentationTimeUs = " + mAudioBufferInfo.presentationTimeUs + ", diff = " + diff);
                        if (diff > 50 && diff < 300) {
                            mIsAudioSought = false;
                        }
                    }


                    internal_write_audio(mAudioOutputBuffers[decoderStatus],
                            0, mAudioBufferInfo.size, mAudioBufferInfo.presentationTimeUs);
                    if (!frameCallback.onFrameAvailable(mAudioBufferInfo.presentationTimeUs, true))
                        mAudioStartTime = adjustPresentationTime(mAudioSync, mAudioStartTime, mAudioBufferInfo.presentationTimeUs);
                }

                mAudioMediaCodec.releaseOutputBuffer(decoderStatus, false);
                if (!mIsAudioSought) {
                    mPreRenderClassAudio.preRender(mAudioBufferInfo.presentationTimeUs);
                }

                if ((mAudioBufferInfo.flags & MediaCodec.BUFFER_FLAG_END_OF_STREAM) != 0) {
                    if (DEBUG) Log.d(TAG, "audio:output EOS");
                    synchronized (mSync) {
                        mAudioOutputDone = true;
                        mSync.notifyAll();
                    }
                }
            }
        }
    }

    /**
     * @param buffer
     * @param offset
     * @param size
     * @param presentationTimeUs
     * @return ignored
     */
    protected boolean internal_write_audio(final ByteBuffer buffer, final int offset, final int size, final long presentationTimeUs) {
//		if (DEBUG) Log.d(TAG, "internal_write_audio");
        if (mAudioOutTempBuf.length < size) {
            mAudioOutTempBuf = new byte[size];
        }
        buffer.position(offset);
        buffer.get(mAudioOutTempBuf, 0, size);
        buffer.clear();
        if (mCallback != null) {
            byte[] buf = new byte[size];
            buffer.get(buf, 0, size);
            mCallback.OnAudioData(buf);
        }
        if (mPlayAudio)
            if (mAudioTrack != null)
                mAudioTrack.write(mAudioOutTempBuf, 0, size);
        return true;
    }

    /**
     * adjusting frame rate
     *
     * @param sync
     * @param startTime
     * @param presentationTimeUs
     * @return startTime
     */
    protected long adjustPresentationTime(final Object sync, final long startTime, final long presentationTimeUs) {
        if (startTime > 0) {
            for (long t = presentationTimeUs - (System.nanoTime() / 1000 - startTime);
                 t > 0; t = presentationTimeUs - (System.nanoTime() / 1000 - startTime)) {
                synchronized (sync) {
                    try {
                        sync.wait(t / 1000, (int) ((t % 1000) * 1000));
                    } catch (final InterruptedException e) {
                    }
                    if ((mState == REQ_STOP) || (mState == REQ_QUIT))
                        break;
                }
            }
            return startTime;
        } else {
            return System.nanoTime() / 1000;
        }
    }

    private final void handleStop() {
        if (DEBUG) Log.v(TAG, "handleStop:");
        synchronized (mVideoTask) {
            internal_stop_video();
            mVideoTrackIndex = -1;
        }
        synchronized (mAudioTask) {
            internal_stop_audio();
            mAudioTrackIndex = -1;
        }
        if (mVideoMediaCodec != null) {
            mVideoMediaCodec.stop();
            mVideoMediaCodec.release();
            mVideoMediaCodec = null;
        }
        if (mAudioMediaCodec != null) {
            mAudioMediaCodec.stop();
            mAudioMediaCodec.release();
            mAudioMediaCodec = null;
        }
        if (mVideoMediaExtractor != null) {
            mVideoMediaExtractor.release();
            mVideoMediaExtractor = null;
        }
        if (mAudioMediaExtractor != null) {
            mAudioMediaExtractor.release();
            mAudioMediaExtractor = null;
        }
        mVideoBufferInfo = mAudioBufferInfo = null;
        mVideoInputBuffers = mVideoOutputBuffers = null;
        mAudioInputBuffers = mAudioOutputBuffers = null;
        if (mMetadata != null) {
            mMetadata.release();
            mMetadata = null;
        }
        synchronized (mSync) {
            mVideoOutputDone = mVideoInputDone = mAudioOutputDone = mAudioInputDone = true;
            mState = STATE_STOP;
        }
        mCallback.onFinished();
    }

    protected void internal_stop_video() {
        if (DEBUG) Log.v(TAG, "internal_stop_video:");
    }

    protected void internal_stop_audio() {
        if (DEBUG) Log.v(TAG, "internal_stop_audio:");
        if (mAudioTrack != null) {
            if (mAudioTrack.getState() != AudioTrack.STATE_UNINITIALIZED)
                mAudioTrack.stop();
            mAudioTrack.release();
            mAudioTrack = null;
        }
        mAudioOutTempBuf = null;
    }

    private final void handlePause() {
        if (DEBUG) Log.v(TAG, "handlePause:");
        synchronized (mSync) {
            if (mState != STATE_PLAYING)
                throw new RuntimeException("invalid state:" + mState
                );
            mState = STATE_PAUSED;
            mPause = true;
        }
        // FIXME unimplemented yet
    }

    private final void handleResume() {
        if (DEBUG) Log.v(TAG, "handleResume:");
        // FIXME unimplemented yet
        synchronized (mSync) {
            if (mState != STATE_PAUSED)
                throw new RuntimeException("invalid state:" + mState);
            mState = STATE_PLAYING;
//            mPrevMonoUsec = 0;
            mPreRenderClass.Reset();
            mPreRenderClassAudio.Reset();
            mPause = false;
        }
    }

    /**
     * search first track index matched specific MIME
     *
     * @param extractor
     * @param mimeType  "video/" or "audio/"
     * @return track index, -1 if not found
     */
    protected static final int selectTrack(final MediaExtractor extractor, final String mimeType) {
        final int numTracks = extractor.getTrackCount();
        MediaFormat format;
        String mime;
        for (int i = 0; i < numTracks; i++) {
            format = extractor.getTrackFormat(i);
            mime = format.getString(MediaFormat.KEY_MIME);
            if (mime.startsWith(mimeType)) {
                if (DEBUG) {
                    Log.d(TAG_STATIC, "Extractor selected track " + i + " (" + mime + "): " + format);
                }
                return i;
            }
        }
        return -1;
    }

    private long mPrevPresentUsec;
    private long mPrevMonoUsec;
    private long mFixedFrameDurationUsec;
    private boolean mLoopReset;
    private static final boolean CHECK_SLEEP_TIME = false;

    private static final long ONE_MILLION = 1000000L;

    public void preRender(long presentationTimeUsec) {
        // For the first frame, we grab the presentation time from the video
        // and the current monotonic clock time.  For subsequent frames, we
        // sleep for a bit to try to ensure that we're rendering frames at the
        // pace dictated by the video stream.
        //
        // If the frame rate is faster than vsync we should be dropping frames.  On
        // Android 4.4 this may not be happening.

        if (mPrevMonoUsec == 0) {
            // Latch current values, then return immediately.
            mPrevMonoUsec = System.nanoTime() / 1000;
            mPrevPresentUsec = presentationTimeUsec;
        } else {
            // Compute the desired time delta between the previous frame and this frame.
            long frameDelta;
            if (mLoopReset) {
                // We don't get an indication of how long the last frame should appear
                // on-screen, so we just throw a reasonable value in.  We could probably
                // do better by using a previous frame duration or some sort of average;
                // for now we just use 30fps.
                mPrevPresentUsec = presentationTimeUsec - ONE_MILLION / 30;
                mLoopReset = false;
            }
            if (mFixedFrameDurationUsec != 0) {
                // Caller requested a fixed frame rate.  Ignore PTS.
                frameDelta = mFixedFrameDurationUsec;
            } else {
                frameDelta = presentationTimeUsec - mPrevPresentUsec;
            }
            if (frameDelta < 0) {
                Log.w(TAG, "Weird, video times went backward");
                frameDelta = 0;
            } else if (frameDelta == 0) {
                // This suggests a possible bug in movie generation.
                Log.i(TAG, "Warning: current frame and previous frame had same timestamp");
            } else if (frameDelta > 10 * ONE_MILLION) {
                // Inter-frame times could be arbitrarily long.  For this player, we want
                // to alert the developer that their movie might have issues (maybe they
                // accidentally output timestamps in nsec rather than usec).
                Log.i(TAG, "Inter-frame pause was " + (frameDelta / ONE_MILLION) +
                        "sec, capping at 5 sec");
                frameDelta = 5 * ONE_MILLION;
            }

            long desiredUsec = mPrevMonoUsec + frameDelta;  // when we want to wake up
            long nowUsec = System.nanoTime() / 1000;
            while (nowUsec < (desiredUsec - 100) /*&& mState == RUNNING*/) {
                // Sleep until it's time to wake up.  To be responsive to "stop" commands
                // we're going to wake up every half a second even if the sleep is supposed
                // to be longer (which should be rare).  The alternative would be
                // to interrupt the thread, but that requires more work.
                //
                // The precision of the sleep call varies widely from one device to another;
                // we may wake early or late.  Different devices will have a minimum possible
                // sleep time. If we're within 100us of the target time, we'll probably
                // overshoot if we try to sleep, so just go ahead and continue on.
                long sleepTimeUsec = desiredUsec - nowUsec;
                if (sleepTimeUsec > 500000) {
                    sleepTimeUsec = 500000;
                }
                try {
                    if (CHECK_SLEEP_TIME) {
                        long startNsec = System.nanoTime();
                        Thread.sleep(sleepTimeUsec / 1000, (int) (sleepTimeUsec % 1000) * 1000);
                        long actualSleepNsec = System.nanoTime() - startNsec;
                        Log.d(TAG, "sleep=" + sleepTimeUsec + " actual=" + (actualSleepNsec / 1000) +
                                " diff=" + (Math.abs(actualSleepNsec / 1000 - sleepTimeUsec)) +
                                " (usec)");
                    } else {
                        Thread.sleep(sleepTimeUsec / 1000, (int) (sleepTimeUsec % 1000) * 1000);
                    }
                } catch (InterruptedException ie) {
                }
                nowUsec = System.nanoTime() / 1000;
            }

            // Advance times using calculated time values, not the post-sleep monotonic
            // clock time, to avoid drifting.
            mPrevMonoUsec += frameDelta;
            mPrevPresentUsec += frameDelta;
        }
    }

    private class PreRenderClass {
        private long mPrevPresentUsec;
        private long mPrevMonoUsec;
        private long mFixedFrameDurationUsec;
        private boolean mLoopReset;
        private static final boolean CHECK_SLEEP_TIME = false;

        private static final long ONE_MILLION = 1000000L;

        public void Reset() {
            mPrevMonoUsec = 0;
        }

        public void preRender(long presentationTimeUsec) {
            // For the first frame, we grab the presentation time from the video
            // and the current monotonic clock time.  For subsequent frames, we
            // sleep for a bit to try to ensure that we're rendering frames at the
            // pace dictated by the video stream.
            //
            // If the frame rate is faster than vsync we should be dropping frames.  On
            // Android 4.4 this may not be happening.

            if (mPrevMonoUsec == 0) {
                // Latch current values, then return immediately.
                mPrevMonoUsec = System.nanoTime() / 1000;
                mPrevPresentUsec = presentationTimeUsec;
            } else {
                // Compute the desired time delta between the previous frame and this frame.
                long frameDelta;
                if (mLoopReset) {
                    // We don't get an indication of how long the last frame should appear
                    // on-screen, so we just throw a reasonable value in.  We could probably
                    // do better by using a previous frame duration or some sort of average;
                    // for now we just use 30fps.
                    mPrevPresentUsec = presentationTimeUsec - ONE_MILLION / 30;
                    mLoopReset = false;
                }
                if (mFixedFrameDurationUsec != 0) {
                    // Caller requested a fixed frame rate.  Ignore PTS.
                    frameDelta = mFixedFrameDurationUsec;
                } else {
                    frameDelta = presentationTimeUsec - mPrevPresentUsec;
                }
                if (frameDelta < 0) {
                    Log.w(TAG, "Weird, video times went backward");
                    frameDelta = 0;
                } else if (frameDelta == 0) {
                    // This suggests a possible bug in movie generation.
                    Log.i(TAG, "Warning: current frame and previous frame had same timestamp");
                } else if (frameDelta > 10 * ONE_MILLION) {
                    // Inter-frame times could be arbitrarily long.  For this player, we want
                    // to alert the developer that their movie might have issues (maybe they
                    // accidentally output timestamps in nsec rather than usec).
                    Log.i(TAG, "Inter-frame pause was " + (frameDelta / ONE_MILLION) +
                            "sec, capping at 5 sec");
                    frameDelta = 5 * ONE_MILLION;
                }

                long desiredUsec = mPrevMonoUsec + frameDelta;  // when we want to wake up
                long nowUsec = System.nanoTime() / 1000;
                while (nowUsec < (desiredUsec - 100) /*&& mState == RUNNING*/) {
                    // Sleep until it's time to wake up.  To be responsive to "stop" commands
                    // we're going to wake up every half a second even if the sleep is supposed
                    // to be longer (which should be rare).  The alternative would be
                    // to interrupt the thread, but that requires more work.
                    //
                    // The precision of the sleep call varies widely from one device to another;
                    // we may wake early or late.  Different devices will have a minimum possible
                    // sleep time. If we're within 100us of the target time, we'll probably
                    // overshoot if we try to sleep, so just go ahead and continue on.
                    long sleepTimeUsec = desiredUsec - nowUsec;
                    if (sleepTimeUsec > 500000) {
                        sleepTimeUsec = 500000;
                    }
                    try {
                        if (CHECK_SLEEP_TIME) {
                            long startNsec = System.nanoTime();
                            Thread.sleep(sleepTimeUsec / 1000, (int) (sleepTimeUsec % 1000) * 1000);
                            long actualSleepNsec = System.nanoTime() - startNsec;
                            Log.d(TAG, "sleep=" + sleepTimeUsec + " actual=" + (actualSleepNsec / 1000) +
                                    " diff=" + (Math.abs(actualSleepNsec / 1000 - sleepTimeUsec)) +
                                    " (usec)");
                        } else {
                            Thread.sleep(sleepTimeUsec / 1000, (int) (sleepTimeUsec % 1000) * 1000);
                        }
                    } catch (InterruptedException ie) {
                    }
                    nowUsec = System.nanoTime() / 1000;
                }

                // Advance times using calculated time values, not the post-sleep monotonic
                // clock time, to avoid drifting.
                mPrevMonoUsec += frameDelta;
                mPrevPresentUsec += frameDelta;
            }
        }

    }


}
