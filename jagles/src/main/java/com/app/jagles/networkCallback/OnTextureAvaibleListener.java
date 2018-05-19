package com.app.jagles.networkCallback;

/**
 * Created by Administrator on 2016/10/13.
 */

public interface OnTextureAvaibleListener {

    public void OnTextureAvaible(int width, int height, byte[] frame, int length, int frame_type,
                                 int timestamp, int index, long ctx);
}
