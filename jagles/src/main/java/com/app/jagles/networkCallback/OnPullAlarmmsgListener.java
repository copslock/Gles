package com.app.jagles.networkCallback;

/**
 * Created by Administrator on 2016/10/13.
 */

public interface OnPullAlarmmsgListener {

    public void OnPullAlarmmsg(int type, int channel, long ticket, String src, String enc, int fps,
                               int width, int height, int samplerate, int samplewidth, int channelAudio,
                               float compress_ratio, int totalLen, int curLen, byte[] dataBuffer);
}
