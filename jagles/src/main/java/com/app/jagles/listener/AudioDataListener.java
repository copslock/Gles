package com.app.jagles.listener;

/**
 * Created by Administrator on 2016/5/20.
 */
public interface AudioDataListener {

    /**
     * 音频数据的回调
     * （已经解码好的）
     * @param data
     */
    public void OnAudioDataListener(byte[] data);
}
