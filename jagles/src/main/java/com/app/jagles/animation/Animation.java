package com.app.jagles.animation;

import com.app.jagles.video.GLVideoRender;

/**
 * Created by Administrator on 2016/5/27.
 */
public class Animation {

    public static final int JA_ANI_TYPE_POSITION = 0;
    public static final int JA_ANI_TYPE_SCALE = 1;
    public static final int JA_ANI_TYPE_ROTATE = 2;

    private GLVideoRender mRender;

    public Animation(GLVideoRender mRender){
        this.mRender = mRender;
    }

    /**
     * 旋转
     * @param angleX
     * @param angleY
     * @param angleZ
     * @param step
     * @param duration
     * @param isloop
     * @param isTexture
     * @param index
     * @param inertia
     * @param msg
     * @return
     */
    public long rotate(float angleX, float angleY, float angleZ,int step, int duration, boolean isloop,boolean isTexture, int index, boolean inertia, int msg)
    {
        float pend[] = new float[3];
        pend[0] = angleX;
        pend[1] = angleY;
        pend[2] = angleZ;

        return mRender.StartAnimation(mRender.mParametricManager, pend,step,duration,isloop,JA_ANI_TYPE_ROTATE,isTexture,index,inertia,msg);
    }

    /**
     * 缩放
     * @param scale
     * @param step
     * @param duration
     * @param isloop
     * @param isTexture
     * @param index
     * @param inertia
     * @param msg
     * @return
     */
    public long scale(float scale, int step, int duration, boolean isloop, boolean isTexture, int index, boolean inertia, int msg)
    {
        float pend[] = new float[3];
        pend[0] = scale;
        pend[1] = scale;
        pend[2] = scale;

        return mRender.StartAnimation(mRender.mParametricManager, pend, step,duration,isloop, JA_ANI_TYPE_SCALE, isTexture,index,inertia,msg);
    }

    public long position(float x, float y, float z,int step, int duration, boolean isloop, boolean isTexture, int index, boolean inertia, int msg)
    {
        float pend[] = new float[3];
        pend[0] = x;
        pend[1] = y;
        pend[2] = z;
        return mRender.StartAnimation(mRender.mParametricManager, pend, step,duration,isloop, JA_ANI_TYPE_POSITION, isTexture,index,inertia,msg);
    }
}
