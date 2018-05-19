package com.app.jagles.connect.listener.searchBackTime;

/**
 * Created by zasko on 2018/1/8.
 */

public interface SearchRecordTimeSubjectListener {
    /**
     * @param abs
     */
    void addObserver(SearchRecordTimeAbs abs);

    /**
     * @param abs
     */
    void removeObserver(SearchRecordTimeAbs abs);


    /**
     *
     * @param startTime
     * @param endTime
     * @param recType  0位：定时录像 1位：移动录像 2位：报警录像 3位：手动录像
     * @param index
     * @param end
     */
    void notifyObserver(int startTime, int endTime, int recType, int index, int end);
}
