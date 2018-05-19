package com.app.jagles.connect.listener.playback;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by zasko on 2018/1/8.
 */

public class FramePlaybackSubjectImpl implements FramePlaybackSubjectListener {

    private List<FramePlaybackAbs> mFramePlaybackList = new ArrayList<>();

    private Object mLock = new Object();

    @Override
    public void addObserver(FramePlaybackAbs abs) {
        if (abs != null) {

            synchronized (mLock) {
                mFramePlaybackList.add(abs);
            }

        }
    }

    @Override
    public void removeObserver(FramePlaybackAbs abs) {
        if (abs != null) {
            synchronized (mLock){
                mFramePlaybackList.remove(abs);
            }
        }
    }

    @Override
    public void notifyObserver(int time, int index) {


        /*这个时间段，有可能造成异常*/
        synchronized (mLock){
            for (FramePlaybackAbs abs : mFramePlaybackList) {
                abs.onFramePlaybackCallBack(time, index);
            }
        }

    }
}
