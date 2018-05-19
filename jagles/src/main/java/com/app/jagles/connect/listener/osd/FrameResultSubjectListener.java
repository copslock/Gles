package com.app.jagles.connect.listener.osd;

/**
 * Created by zasko on 2018/1/8.
 */

public interface FrameResultSubjectListener {
    void addObserver(FrameResultAbs abs);

    void removeObserver(FrameResultAbs abs);

    void notifyObserver(int width, int height, long frame, int length, long time);

    void notifyObserver(int width, int scene, long time);
}
