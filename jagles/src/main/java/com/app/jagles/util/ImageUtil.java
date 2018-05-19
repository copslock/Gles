package com.app.jagles.util;

import android.graphics.Bitmap;
import android.util.Log;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileOutputStream;

/**
 * Created by Administrator on 2016/6/12.
 */
public class ImageUtil {

    private static final String TAG = "ImageUtil";


    /**
     * 压缩图片文件
     *
     * @param image
     * @param file
     */
    public static void compressImage(Bitmap image, File file, int max) {


        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        image.compress(Bitmap.CompressFormat.JPEG, 100, baos);
        int options = 100;
        while (baos.toByteArray().length / 1024 > max) {
            baos.reset();
            if (options < 0) {
                options = 10;
                image.compress(Bitmap.CompressFormat.JPEG, options, baos);
                break;
            }
            image.compress(Bitmap.CompressFormat.JPEG, options, baos);
            options -= 10;
        }
        try {
            FileOutputStream fos = new FileOutputStream(file);
            fos.write(baos.toByteArray());
            fos.flush();
            fos.close();
            image.recycle();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }


    /**
     * 判断有没足够内存截图
     *
     * @param needSize
     * @return
     */
    public static boolean hasEnoughMemoryToCapture(int needSize) {
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
        if (canUseMemory >= needSize) {
            return true;
        }
        return false;
    }
}
