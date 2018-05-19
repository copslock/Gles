package com.app.jagles.util;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.text.TextUtils;
import android.util.Log;

import java.io.File;

/**
 * Created by zasko on 2018/3/12.
 */

public class DisplayUtil {

    private static final String TAG = "DisplayUtil";

    public static final int MIN_MEMORY = 50 * 1024 * 1024;

    public static final int[] SPLIT_MODE = {1, 4, 6, 8, 9, 13, 16};


    public static int getSplitMode(int cameraList) {

        int split = 0;
        for (int i = 0; i < SPLIT_MODE.length; i++) {
            if (cameraList == SPLIT_MODE[i]) {
                split = i;
                break;
            }
            if (SPLIT_MODE[i] > cameraList) {
                split = i - 1;
                break;
            }
        }
        return split;
    }


    public static void compressImage(String fileName) {

        if (TextUtils.isEmpty(fileName)) {
            return;
        }

        File file = new File(fileName);
        if (!file.exists()) {
            Log.d(TAG, "compressImage: ---->" + file);
            return;
        }


        Bitmap bitmap = BitmapFactory.decodeFile(fileName);
        if (bitmap != null) {
            ImageUtil.compressImage(bitmap, file, 20);
        }

    }


    public static boolean enoughMemory(int min) {
        //最大内存
        long maxMemory = Runtime.getRuntime().maxMemory();
        //分配的可用内存
        long freeMemory = Runtime.getRuntime().freeMemory();
        //已用内存
        long usedMemory = Runtime.getRuntime().totalMemory() - freeMemory;
        //剩下可使用的内存
        long canUseMemory = maxMemory - usedMemory;
        Log.d(TAG, "hasEnoughMemoryToCapture: " +
                "maxMemory = " + maxMemory +
                ", freeMemory = " + freeMemory +
                ", usedMemory = " + usedMemory +
                ", canUseMemory = " + canUseMemory);

        if (canUseMemory > min) {
            return true;
        }
        return false;

    }
}
