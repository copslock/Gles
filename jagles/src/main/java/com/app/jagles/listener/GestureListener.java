package com.app.jagles.listener;

import android.view.ScaleGestureDetector;

/**
 * Created by MiQi on 2016/6/13.
 */
public interface GestureListener {

    /**
     * @param pageCount
     * @param currentPage
     * @param screenMode
     * @param isRecording
     * @param bitrate
     */
    public void onSingleClick(int pageCount,
                              int currentPage,
                              int screenMode,
                              boolean isRecording,
                              int bitrate,
                              int window);

    /**
     * @param pageCount
     * @param currentPage
     * @param screenMode
     * @param isRecording
     * @param bitrate
     */
    public void onDoubleClick(int pageCount,
                              int currentPage,
                              int screenMode,
                              boolean isRecording,
                              int bitrate,
                              int window);

    /**
     * @param pageCount
     * @param currentPage
     * @param screenMode
     * @param isRecording
     * @param bitrate
     */
    public void onPageChange(int pageCount,
                             int currentPage,
                             int screenMode,
                             boolean isRecording,
                             int bitrate,
                             int window);

    public void onScale(ScaleGestureDetector detector);

    public void onScroll();

    void onUp();
}
