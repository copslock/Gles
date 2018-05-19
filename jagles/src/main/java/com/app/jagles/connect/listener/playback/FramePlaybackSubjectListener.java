package com.app.jagles.connect.listener.playback;

/**
 * Created by zasko on 2018/1/8.
 */

public interface FramePlaybackSubjectListener {

    /**
     * @param abs
     */
    void addObserver(FramePlaybackAbs abs);

    /**
     * @param abs
     */
    void removeObserver(FramePlaybackAbs abs);

    /**
     * @param time
     * @param index
     */
    void notifyObserver(int time, int index);
}
