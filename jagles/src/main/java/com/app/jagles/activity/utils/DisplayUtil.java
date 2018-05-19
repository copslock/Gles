package com.app.jagles.activity.utils;

import android.content.Context;

/**
 * Created by zasko on 2018/1/10.
 */

public final class DisplayUtil {

    public static int dp2px(Context context, float dpValue) {
        float scale = context.getResources().getDisplayMetrics().density;
        return (int) (dpValue * scale + 0.5f);
    }
}
