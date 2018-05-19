package com.app.jagles.connect.listener.osd;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by zasko on 2018/1/8.
 */

public class FrameResultSubjectImpl implements FrameResultSubjectListener {

    private List<FrameResultAbs> mFrameResultList = new ArrayList<>();

    private Object mLock = new Object();

    @Override
    public void addObserver(FrameResultAbs abs) {
        if (abs != null) {
            synchronized (mLock) {
                mFrameResultList.add(abs);
            }

        }
    }

    @Override
    public void removeObserver(FrameResultAbs abs) {
        if (abs != null) {
            synchronized (mLock) {
                mFrameResultList.remove(abs);
            }

        }

    }

    @Override
    public void notifyObserver(int width, int height, long frame, int length, long time) {

        synchronized (mLock){
            for (FrameResultAbs abs : mFrameResultList) {
                abs.onFrameResultCallBack(width, height, frame, length, time);
            }
        }
    }

    @Override
    public void notifyObserver(int installMode, int scene, long time) {
        synchronized (mLock){
            for (FrameResultAbs abs : mFrameResultList) {
                abs.onOOBFrameResultCallBack(installMode, scene, time);
            }
        }
    }
}
