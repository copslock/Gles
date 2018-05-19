package com.app.jagles.view;

import javax.microedition.khronos.opengles.GL10;

/**
 * Created by zasko on 2018/1/9.
 */

public interface OnGLDisplayCreateListener {

    void onGLDisplayCreateCallBack(GL10 unused, long handle, int width, int height);
}
