package com.app.jagles.eventbus;

/**
 * Created by JuAn_Zhangsongzhou on 2017/7/19.
 */

public class GLPlayFrameComeBackEventBus {

    public static GLPlayFrameComeBackEventBus getInstance() {
        return Holder.instance;
    }

    private static final class Holder {
        private static final GLPlayFrameComeBackEventBus instance = new GLPlayFrameComeBackEventBus();
    }

    private OnFrameComeBackListener mFrameListener;

    public OnFrameComeBackListener getOnFrameComeBackListener() {
        return mFrameListener;
    }

    public void setOnFrameComeBackListener(OnFrameComeBackListener listener) {
        this.mFrameListener = listener;
    }

    public interface OnFrameComeBackListener {

        void onFrameComeBack(boolean stats);
    }


}
