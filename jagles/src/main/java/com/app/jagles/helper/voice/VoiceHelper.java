package com.app.jagles.helper.voice;

import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.util.Log;

import com.app.jagles.audio.BytesTransUtil;
import com.app.jagles.audio.PCMA;
import com.app.jagles.connect.JAConnector;
import com.app.jagles.video.Manager;

import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.atomic.AtomicBoolean;

/**
 * Created by zasko on 2018/1/9.
 */

public class VoiceHelper {

    private static final String TAG = "VoiceHelper";

    /**
     * 录音要用到的类
     */
    private AudioRecord mRecorder;
    private byte[] pcm;
    private int mRecorderBufferSize;
    private AtomicBoolean isStartRecord = new AtomicBoolean();
    private AtomicBoolean isSendAudio = new AtomicBoolean();

    private VoiceHelper() {

    }

    public boolean isRecord;

    private int mCurrentChannel = 0;
    private int mCurrentIndex = 0;

    public static VoiceHelper create() {
        VoiceHelper mHelper = new VoiceHelper();
        mHelper.initRecorder();
        return mHelper;
    }

    /**
     * 录音数队列
     */
    private ConcurrentLinkedQueue<byte[]> audioQueue = new ConcurrentLinkedQueue<byte[]>();

    /**
     * 初始化录音
     */
    public void initRecorder() {
        mRecorderBufferSize = AudioRecord.getMinBufferSize(8000,
                AudioFormat.CHANNEL_IN_MONO, AudioFormat.ENCODING_PCM_16BIT);
        pcm = new byte[320];
        mRecorder = new AudioRecord(
                MediaRecorder.AudioSource.MIC,
                8000,
                AudioFormat.CHANNEL_IN_MONO,
                AudioFormat.ENCODING_PCM_16BIT,
                mRecorderBufferSize);
    }

    /**
     * @param key
     */
    public void startTalking(String key) {
        isRecord = true;
        sendAudioThread(key);
        recordThread();
    }


    /**
     *
     */
    public void recordThread() {
        ThreadPool.initialize();
        ThreadPool.execute(new Runnable() {
            @Override
            public void run() {
                isStartRecord.set(true);
                if (mRecorder == null) {
                    initRecorder();
                }
                try {
                    mRecorder.startRecording();
                } catch (RuntimeException r) {
                    r.printStackTrace();
                    return;
                }
                audioQueue.clear();
                while (isStartRecord.get()) {
                    if (mRecorder == null) {
                        break;
                    }
                    Log.d(TAG, "run: --------------------------->pcm: " + pcm.length);
                    int readSize = mRecorder.read(pcm, 0, pcm.length);
                    Log.d(TAG, "run: --------------------------->readSize: " + readSize);
                    if (readSize >= 0) {
                        byte[] alawData = new byte[readSize / 2];
                        if (readSize > 0) {
                            alawData = PCMA.linear2alaw(                        //pcm转ALAW
                                    BytesTransUtil.Bytes2Shorts(pcm), 0, alawData,
                                    readSize);
                            Log.d(TAG, "run: --------------------------->" + alawData.length);
                            if (alawData.length > 0) {
                                audioQueue.add(alawData);
                            }
                        }
                    }
                }
            }
        });
    }

    /**
     * @param key
     */
    public void sendAudioThread(final String key) {
        ThreadPool.initialize();
        ThreadPool.execute(new Runnable() {
            @Override
            public void run() {
                int ret = JAConnector.callDeviceAudio(key, mCurrentChannel, mCurrentIndex);
                if (ret == 0) {
                    isSendAudio.set(true);
                }
                while (isSendAudio.get()) {
                    byte[] alawData = audioQueue.poll();
                    if (alawData != null) {
                        Log.d(TAG, "run: --------------------------->HelperVoice----->" + alawData.length);
                        JAConnector.sendAudioPacket(key, alawData, alawData.length, 20, "G711A", 8000, 16, 1, 2.0f, mCurrentIndex);
                    }
                    if (mRecorder == null && audioQueue.size() == 0) {

                        Log.d(TAG, "run: ---------->sendAudio end!");
                        break;
                    }
                }
                isSendAudio.set(false);
                JAConnector.handUpDeviceAudio(key, mCurrentIndex);
                audioQueue.clear();
            }
        });
    }

    /**
     * 停止语音对讲
     *
     * @param key
     */
    public void endTalking(String key) {
        isRecord = false;
        isStartRecord.set(false);
        if (mRecorder != null) {
            try {
                mRecorder.stop();
                mRecorder.release();
            } catch (RuntimeException r) {
                JAConnector.handUpDeviceAudio(key, mCurrentIndex);
                mRecorder = null;
                isSendAudio.set(false);
                return;
            }
        }
        mRecorder = null;
    }


    public int getCurrentChannel() {
        return mCurrentChannel;
    }

    public void setCurrentChannel(int channel) {
        this.mCurrentChannel = channel;
    }

    public int getCurrentIndex() {
        return mCurrentIndex;
    }

    public void setCurrentIndex(int index) {
        this.mCurrentIndex = index;
    }
}
