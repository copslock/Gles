package com.app.jagles.HWCodec;

import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.media.MediaMuxer;
import android.os.Build;
import android.os.Bundle;
import android.support.annotation.RequiresApi;
import android.util.Log;
import android.view.Surface;

import java.io.File;
import java.io.IOException;
import java.nio.ByteBuffer;

/**
 * This class wraps up the core components used for surface-input video encoding.
 * <p>
 * Once created, frames are fed to the input surface.  Remember to provide the presentation
 * time stamp, and always call drainEncoder() before swapBuffers() to ensure that the
 * producer side doesn't get backed up.
 * <p>
 * This class is not thread-safe, with one exception: it is valid to use the input surface
 * on one thread, and drain the output on a different thread.
 */
public class VideoEncoderCore {
    private static final String TAG = "honglee_0704";
    private static final boolean VERBOSE = true;

    // TODO: these ought to be configurable as well
    private static final String MIME_TYPE = "video/avc";    // H.264 Advanced Video Coding
    private static final String AUDIO_MIME_TYPE = "audio/mp4a-latm";
    private static final int FRAME_RATE = 30;               // 30fps
    private static final int IFRAME_INTERVAL = 2;           // 5 seconds between I-frames

    private Surface mInputSurface;
    private MediaMuxerWrapper mMuxerWrapper;
    private MediaCodec.BufferInfo mVideoBufferInfo;
    private MediaCodec.BufferInfo mAudioBufferInfo;

    private TrackInfo mVideoTrackInfo;
    private TrackInfo mAudioTrackInfo;
    private MediaCodec mVideoEncoder;
    private MediaCodec mAudioEncoder;
    private MediaFormat mVideoFormat;
    private MediaFormat mAudioFormat;
    private MediaFormat mVideoOutputFormat;
    private MediaFormat mAudioOutputFormat;
    boolean firstFrameReady = false;
    boolean audioEosRequested = false;
    private boolean fullStopReceived = false;
    long startWhen;
    boolean eosSentToAudioEncoder = false;
    boolean haveAudioData = false;

    // Audio
    public static final int SAMPLE_RATE = 8000;
    public static final int SAMPLES_PER_FRAME = 1024; // AAC
    public static final int CHANNEL_CONFIG = AudioFormat.CHANNEL_IN_MONO;
    public static final int AUDIO_FORMAT = AudioFormat.ENCODING_PCM_16BIT;
    //private AudioRecord audioRecord;
    private long lastEncodedAudioTimeStamp = 0;
    int frameCount = 0;
    boolean eosSentToVideoEncoder = false;
    boolean firstRun = true;


    /**
     * Configures encoder and muxer state, and prepares the input Surface.
     */
    @RequiresApi(api = Build.VERSION_CODES.LOLLIPOP)
    public VideoEncoderCore(int width, int height, int bitRate, File outputFile)
            throws IOException {

        eosSentToVideoEncoder = false;
        mVideoBufferInfo = new MediaCodec.BufferInfo();
        mVideoTrackInfo = new TrackInfo();

        Log.d(TAG, "createVideoFormat: width = " + width + ", height = " + height);
        mVideoEncoder = MediaCodec.createEncoderByType(MIME_TYPE);
        createVideoFormat(width, height, bitRate, width / 10, height / 10);

        mInputSurface = mVideoEncoder.createInputSurface();
        mVideoEncoder.start();


        mAudioBufferInfo = new MediaCodec.BufferInfo();
        mAudioTrackInfo = new TrackInfo();

        mAudioFormat = new MediaFormat();
        mAudioFormat.setString(MediaFormat.KEY_MIME, AUDIO_MIME_TYPE);
        mAudioFormat.setInteger(MediaFormat.KEY_AAC_PROFILE, MediaCodecInfo.CodecProfileLevel.AACObjectLC);
        mAudioFormat.setInteger(MediaFormat.KEY_SAMPLE_RATE, 8000);
        mAudioFormat.setInteger(MediaFormat.KEY_CHANNEL_COUNT, 1);
        mAudioFormat.setInteger(MediaFormat.KEY_BIT_RATE, 64000);
        mAudioFormat.setInteger(MediaFormat.KEY_MAX_INPUT_SIZE, 16384);

        mAudioEncoder = MediaCodec.createEncoderByType(AUDIO_MIME_TYPE);
        mAudioEncoder.configure(mAudioFormat, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);
        mAudioEncoder.start();


        mMuxerWrapper = new MediaMuxerWrapper(outputFile, MediaMuxer.OutputFormat.MUXER_OUTPUT_MPEG_4);

        mVideoTrackInfo.index = -1;
        mVideoTrackInfo.muxerWrapper = mMuxerWrapper;
        mAudioTrackInfo.index = -1;
        mAudioTrackInfo.muxerWrapper = mMuxerWrapper;
    }


    private void createVideoFormat(int width, int height, int bitRate, int widthRoot, int heightRoot) throws IOException {
        mVideoFormat = MediaFormat.createVideoFormat(MIME_TYPE, width, height);

        // Set some properties.  Failing to specify some of these can cause the MediaCodec
        // configure() call to throw an unhelpful exception.
        mVideoFormat.setInteger(MediaFormat.KEY_COLOR_FORMAT,
                MediaCodecInfo.CodecCapabilities.COLOR_FormatSurface);
        mVideoFormat.setInteger(MediaFormat.KEY_BIT_RATE, bitRate);
        mVideoFormat.setInteger(MediaFormat.KEY_FRAME_RATE, FRAME_RATE);
        mVideoFormat.setInteger(MediaFormat.KEY_I_FRAME_INTERVAL, IFRAME_INTERVAL);
        if (VERBOSE) Log.d(TAG, "format: " + mVideoFormat);

        // Create a MediaCodec encoder, and configure it with our format.  Get a Surface
        // we can use for input and wrap it with a class that handles the EGL work.
        try {
            mVideoEncoder.configure(mVideoFormat, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);
            Log.d(TAG, "createVideoFormat: width = " + width + ", height = " + height);
        } catch (MediaCodec.CodecException e) {
            createVideoFormat(width - widthRoot, height - heightRoot, bitRate, widthRoot, heightRoot);
        }
    }


    /**
     * Returns the encoder's input surface.
     */
    public Surface getInputSurface() {
        return mInputSurface;
    }

    public void stopRecording() {
        fullStopReceived = true;
        drainEncoder(mVideoEncoder, mVideoBufferInfo, mVideoTrackInfo, fullStopReceived);
        ByteBuffer buf = ByteBuffer.allocate(2048);
        sendAudioToEncoder(buf.array());
        drainEncoder(mAudioEncoder, mAudioBufferInfo, mAudioTrackInfo, fullStopReceived);
    }

    public void startRecording() {

        fullStopReceived = false;
        haveAudioData = false;

        if (firstRun) {
            setupAudioRecord();
            startAudioRecord();
            firstFrameReady = true;

            startWhen = System.nanoTime();
            firstRun = false;
        }

        try {
            drainEncoder(mVideoEncoder, mVideoBufferInfo, mVideoTrackInfo, fullStopReceived);
        } catch (NullPointerException e) {
            e.printStackTrace();
        }


    }

    private void setupAudioRecord() {
//        int min_buffer_size = AudioRecord.getMinBufferSize(SAMPLE_RATE, CHANNEL_CONFIG, AUDIO_FORMAT);
//        int buffer_size = SAMPLES_PER_FRAME * 10;
//        if (buffer_size < min_buffer_size)
//            buffer_size = ((min_buffer_size / SAMPLES_PER_FRAME) + 1) * SAMPLES_PER_FRAME * 2;
//
//        audioRecord = new AudioRecord(
//                MediaRecorder.AudioSource.MIC,       // source
//                SAMPLE_RATE,                         // sample rate, hz
//                CHANNEL_CONFIG,                      // channels
//                AUDIO_FORMAT,                        // audio format
//                buffer_size);                        // buffer size (bytes)
    }

    public void P_drainEncoder(boolean video, boolean endstream) {
        if (video)
            drainEncoder(mVideoEncoder, mVideoBufferInfo, mVideoTrackInfo, endstream);
        else {
            if (!firstFrameReady)
                return;

            synchronized (mAudioTrackInfo.muxerWrapper.sync) {
                drainEncoder(mAudioEncoder, mAudioBufferInfo, mAudioTrackInfo, fullStopReceived);
            }
        }
    }

    private void startAudioRecord() {
        //if(audioRecord != null){

        drainEncoder(mAudioEncoder, mAudioBufferInfo, mAudioTrackInfo, false);
//            new Thread(new Runnable(){
//
//                @Override
//                public void run() {
//                    //audioRecord.startRecording();
//                    boolean audioEosRequestedCopy = false;
//                    while(true){
//
//                        if(!firstFrameReady)
//                            continue;
//                        audioEosRequestedCopy = audioEosRequested; // make sure audioEosRequested doesn't change value mid loop
//                        if (audioEosRequestedCopy || fullStopReceived){ // TODO post eosReceived message with Handler?
//                            Log.i(TAG, "Audio loop caught audioEosRequested / fullStopReceived " + audioEosRequestedCopy + " " + fullStopReceived);
//                            //sendAudioToEncoder(true);
//                        }
//                        if (fullStopReceived){
//                            Log.i(TAG, "Stopping AudioRecord");
//                            //audioRecord.stop();
//                        }
//
//                        synchronized (mAudioTrackInfo.muxerWrapper.sync){
//                            drainEncoder(mAudioEncoder, mAudioBufferInfo, mAudioTrackInfo, audioEosRequestedCopy || fullStopReceived);
//                        }
//
//                        if (audioEosRequestedCopy) audioEosRequested = false;
//
//                        if (!fullStopReceived){
//                            ;//sendAudioToEncoder(false);
//
//                        }else{
//                            break;
//                        }
//                    } // end while
//                }
//            }).start();

        //}

    }

    int audioInputLength;
    long audioAbsolutePtsUs;

    public void sendAudioToEncoder(byte[] buffer) {
        // send current frame data to encoder
        boolean endOfStream = fullStopReceived;
        haveAudioData = true;
        try {
            ByteBuffer[] inputBuffers = mAudioEncoder.getInputBuffers();
            int inputBufferIndex = mAudioEncoder.dequeueInputBuffer(-1);
            if (inputBufferIndex >= 0) {
                ByteBuffer inputBuffer = inputBuffers[inputBufferIndex];
                inputBuffer.clear();

                //TODO:ʵ����Ƶ����
                //audioInputLength = audioRecord.read(inputBuffer, SAMPLES_PER_FRAME * 2);
                inputBuffer.position(0);
                inputBuffer.put(buffer);
                audioInputLength = buffer.length;
                audioAbsolutePtsUs = (System.nanoTime()) / 1000L;

                // We divide audioInputLength by 2 because audio samples are
                // 16bit.
                audioAbsolutePtsUs = getJitterFreePTS(audioAbsolutePtsUs, audioInputLength / 2);

                if (audioInputLength == AudioRecord.ERROR_INVALID_OPERATION)
                    Log.e(TAG, "Audio read error: invalid operation");
                if (audioInputLength == AudioRecord.ERROR_BAD_VALUE)
                    Log.e(TAG, "Audio read error: bad value");

                if (endOfStream) {
                    Log.i(TAG, "EOS received in sendAudioToEncoder");
                    mAudioEncoder.queueInputBuffer(inputBufferIndex, 0, audioInputLength, audioAbsolutePtsUs, MediaCodec.BUFFER_FLAG_END_OF_STREAM);
                    eosSentToAudioEncoder = true;
                } else {
                    mAudioEncoder.queueInputBuffer(inputBufferIndex, 0, audioInputLength, audioAbsolutePtsUs, 0);
                }
            }
        } catch (Throwable t) {
            Log.e(TAG, "_offerAudioEncoder exception");
            t.printStackTrace();
        }
    }

    /**
     * Ensures that each audio pts differs by a constant amount from the previous one.
     *
     * @param bufferPts presentation timestamp in us
     * @param bufferSamplesNum the number of samples of the buffer's frame
     * @return
     */

    long startPTS = 0;
    long totalSamplesNum = 0;

    private long getJitterFreePTS(long bufferPts, long bufferSamplesNum) {
        long correctedPts = 0;
        long bufferDuration = (1000000 * bufferSamplesNum) / (SAMPLE_RATE);
        bufferPts -= bufferDuration; // accounts for the delay of acquiring the audio buffer
        if (totalSamplesNum == 0) {
            // reset
            startPTS = bufferPts;
            totalSamplesNum = 0;
        }
        correctedPts = startPTS + (1000000 * totalSamplesNum) / (SAMPLE_RATE);
        if (bufferPts - correctedPts >= 2 * bufferDuration) {
            // reset
            startPTS = bufferPts;
            totalSamplesNum = 0;
            correctedPts = startPTS;
        }
        totalSamplesNum += bufferSamplesNum;
        return correctedPts;
    }

    /**
     * Releases encoder resources.
     */
    public void release() {
        if (VERBOSE) Log.d(TAG, "releasing encoder objects");

        mMuxerWrapper.stop();

    }

    /**
     * Extracts all pending data from the encoder and forwards it to the muxer.
     * <p>
     * If endOfStream is not set, this returns when there is no more data to drain.  If it
     * is set, we send EOS to the encoder, and then iterate until we see EOS on the output.
     * Calling this with endOfStream set should be done once, right before stopping the muxer.
     * <p>
     * We're just using the muxer to get a .mp4 file (instead of a raw H.264 stream).  We're
     * not recording audio.
     */
    private void drainEncoder(MediaCodec encoder, MediaCodec.BufferInfo bufferInfo, TrackInfo trackInfo, boolean endOfStream) {
        final int TIMEOUT_USEC = 100;

        if(encoder == null)
            return;

        //TODO: Get Muxer from trackInfo
        MediaMuxerWrapper muxerWrapper = trackInfo.muxerWrapper;

        if (VERBOSE)
            Log.d(TAG, "drain" + ((encoder == mVideoEncoder) ? "Video" : "Audio") + "Encoder(" + endOfStream + ")");
        if (endOfStream && encoder == mVideoEncoder) {
            if (VERBOSE)
                Log.d(TAG, "sending EOS to " + ((encoder == mVideoEncoder) ? "video" : "audio") + " encoder");
            if (encoder != null)
                encoder.signalEndOfInputStream();
            eosSentToVideoEncoder = true;
        }
        //testing
        ByteBuffer[] encoderOutputBuffers = null;
        try {
            encoderOutputBuffers = encoder.getOutputBuffers();
        } catch (Exception e) {
            e.printStackTrace();
        }

        while (true) {
            int encoderStatus = -10;
            try {
                encoderStatus = encoder.dequeueOutputBuffer(bufferInfo, TIMEOUT_USEC);
            } catch (Exception e) {
                e.printStackTrace();
                break;
            }

            if (encoderStatus == MediaCodec.INFO_TRY_AGAIN_LATER) {
                // no output available yet
                if (!endOfStream) {
                    if (VERBOSE) Log.d(TAG, "no output available. aborting drain");
                    break;      // out of while
                } else {
                    if (encoder != mVideoEncoder) {
                        continue;
                    }

                    muxerWrapper.finishTrack();
                    if (VERBOSE)
                        Log.d(TAG, "end of " + ((encoder == mVideoEncoder) ? " video" : " audio") + " stream reached. ");

                    if (fullStopReceived) {
                        if (encoder == mVideoEncoder) {
                            Log.i(TAG, "Stopping and releasing video encoder");
                            stopAndReleaseVideoEncoder();
                        } else if (encoder == mAudioEncoder) {
                            Log.i(TAG, "Stopping and releasing audio encoder");
                            stopAndReleaseAudioEncoder();
                        }
                        //stopAndReleaseEncoders();
                        break;
                    }
                    //if (VERBOSE) Log.d(TAG, "no output available, spinning to await EOS");
                }
            } else if (encoderStatus == MediaCodec.INFO_OUTPUT_BUFFERS_CHANGED) {
                // not expected for an encoder
                try {
                    encoderOutputBuffers = encoder.getOutputBuffers();
                } catch (Exception e) {
                    e.printStackTrace();
                    continue;
                }

            } else if (encoderStatus == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
                // should happen before receiving buffers, and should only happen once

                if (muxerWrapper.started) {
                    //Log.e(TAG, "format changed after muxer start! Can we ignore?");
                    //throw new RuntimeException("format changed after muxer start");
                } else {
                    MediaFormat newFormat = encoder.getOutputFormat();
                    if (encoder == mVideoEncoder)
                        mVideoOutputFormat = newFormat;
                    else if (encoder == mAudioEncoder)
                        mAudioOutputFormat = newFormat;

                    // now that we have the Magic Goodies, start the muxer
                    trackInfo.index = muxerWrapper.addTrack(newFormat);
                    if (!muxerWrapper.allTracksAdded())
                        break;  // Allow both encoders to send output format changed before attempting to write samples
                }

            } else if (encoderStatus < 0) {
                Log.w(TAG, "unexpected result from encoder.dequeueOutputBuffer: " +
                        encoderStatus);
                // let's ignore it
            } else {
                if (encoderOutputBuffers == null) {
                    continue;
                }

                ByteBuffer encodedData = encoderOutputBuffers[encoderStatus];
                if (encodedData == null) {
                    throw new RuntimeException("encoderOutputBuffer " + encoderStatus +
                            " was null");
                }

                if ((bufferInfo.flags & MediaCodec.BUFFER_FLAG_CODEC_CONFIG) != 0) {
                    // The codec config data was pulled out and fed to the muxer when we got
                    // the INFO_OUTPUT_FORMAT_CHANGED status.  Ignore it.
                    if (VERBOSE) Log.d(TAG, "ignoring BUFFER_FLAG_CODEC_CONFIG");
                    bufferInfo.size = 0;
                }


                if (bufferInfo.size != 0) {
                    if (!trackInfo.muxerWrapper.started) {
                        Log.e(TAG, "Muxer not started. dropping " + ((encoder == mVideoEncoder) ? " video" : " audio") + " frames");
                        //throw new RuntimeException("muxer hasn't started");
                    } else {
                        // adjust the ByteBuffer values to match BufferInfo (not needed?)
                        encodedData.position(bufferInfo.offset);
                        encodedData.limit(bufferInfo.offset + bufferInfo.size);
                        if (encoder == mAudioEncoder) {
                            if (bufferInfo.presentationTimeUs < lastEncodedAudioTimeStamp)
                                bufferInfo.presentationTimeUs = lastEncodedAudioTimeStamp += 23219; // Magical AAC encoded frame time
                            lastEncodedAudioTimeStamp = bufferInfo.presentationTimeUs;
                        }
                        if (bufferInfo.presentationTimeUs < 0) {
                            bufferInfo.presentationTimeUs = 0;
                        }

                        Log.d("honglee_0704", "trackInfo.index:" + trackInfo.index + "-----" + bufferInfo);
                        muxerWrapper.muxer.writeSampleData(trackInfo.index, encodedData, bufferInfo);

                        if (VERBOSE)
                            Log.d(TAG, "sent " + bufferInfo.size + ((encoder == mVideoEncoder) ? " video" : " audio") + " bytes to muxer with pts " + bufferInfo.presentationTimeUs);

                    }
                }

                encoder.releaseOutputBuffer(encoderStatus, false);

                if ((bufferInfo.flags & MediaCodec.BUFFER_FLAG_END_OF_STREAM) != 0) {
                    if (!endOfStream) {
                        Log.w(TAG, "reached end of stream unexpectedly");
                    } else {

                        if (VERBOSE)
                            Log.d(TAG, "end of " + ((encoder == mVideoEncoder) ? " video" : " audio") + " stream reached. ");
                        if (!fullStopReceived) {
                        } else {
                            muxerWrapper.finishTrack();
                            if (encoder == mVideoEncoder) {
                                Log.i(TAG, "Stopping and releasing video encoder");
                                stopAndReleaseVideoEncoder();
                            } else if (encoder == mAudioEncoder) {
                                Log.i(TAG, "Stopping and releasing audio encoder");
                                stopAndReleaseAudioEncoder();
                            }
                            //stopAndReleaseEncoders();
                        }
                    }
                    break;      // out of while
                }
            }
        }
    }

    private void stopAndReleaseVideoEncoder() {
        eosSentToVideoEncoder = false;
        frameCount = 0;
        if (mVideoEncoder != null) {
            try {
                mVideoEncoder.stop();
            } catch (IllegalStateException e) {
                e.printStackTrace();
            }
            mVideoEncoder.release();
            mVideoEncoder = null;
        }
    }


    private void stopAndReleaseAudioEncoder() {
        lastEncodedAudioTimeStamp = 0;
        eosSentToAudioEncoder = false;

        if (mAudioEncoder != null) {
            mAudioEncoder.stop();
            mAudioEncoder.release();
//            mAudioEncoder = null;
        }
    }

    class TrackInfo {
        int index = 0;
        MediaMuxerWrapper muxerWrapper;
    }

    class MediaMuxerWrapper {
        MediaMuxer muxer;
        final int TOTAL_NUM_TRACKS = 2;
        boolean started = false;
        int numTracksAdded = 0;
        int numTracksFinished = 0;

        Object sync = new Object();

        public MediaMuxerWrapper(File outputFile, int format) {
            restart(outputFile, format);

        }

        public int TrackCount() {
            return numTracksAdded;
        }

        public int addTrack(MediaFormat format) {
            numTracksAdded++;
            int trackIndex = muxer.addTrack(format);
            if (numTracksAdded == TOTAL_NUM_TRACKS) {
                if (VERBOSE)
                    Log.i(TAG, "All tracks added, starting " + ((this == mMuxerWrapper) ? "muxer1" : "muxer2") + "!");
                muxer.start();
                started = true;
            }
            return trackIndex;
        }

        public void finishTrack() {
            numTracksFinished++;
            if (numTracksFinished == TOTAL_NUM_TRACKS) {
                if (VERBOSE)
                    Log.i(TAG, "All tracks finished, stopping " + ((this == mMuxerWrapper) ? "muxer1" : "muxer2") + "!");
                stop();
            }

        }

        public boolean allTracksAdded() {
            return (numTracksAdded == TOTAL_NUM_TRACKS);
        }

        public boolean allTracksFinished() {
            return (numTracksFinished == TOTAL_NUM_TRACKS);
        }


        public void stop() {
            if (muxer != null) {
                try {
                    if (!allTracksFinished()) Log.e(TAG, "Stopping Muxer before all tracks added!");
                    if (!started) Log.e(TAG, "Stopping Muxer before it was started");
                    muxer.stop();
                    muxer.release();
                    muxer = null;
                    started = false;
                    numTracksAdded = 0;
                    numTracksFinished = 0;
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }


        private void restart(File outputFile, int format) {
            stop();
            try {
                muxer = new MediaMuxer(outputFile.toString(), format);
            } catch (IOException e) {
                throw new RuntimeException("MediaMuxer creation failed", e);
            }
        }
    }
}
