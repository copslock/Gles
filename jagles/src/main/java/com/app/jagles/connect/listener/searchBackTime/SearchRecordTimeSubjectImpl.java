package com.app.jagles.connect.listener.searchBackTime;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

/**
 * Created by zasko on 2018/1/8.
 */

public class SearchRecordTimeSubjectImpl implements SearchRecordTimeSubjectListener {

    private List<SearchRecordTimeAbs> mSearchBackTimeAbsList = Collections.synchronizedList(new ArrayList<SearchRecordTimeAbs>());

    @Override
    public void addObserver(SearchRecordTimeAbs abs) {
        if (abs != null) {
            mSearchBackTimeAbsList.add(abs);
        }

    }

    @Override
    public void removeObserver(SearchRecordTimeAbs abs) {
        if (abs != null) {
            mSearchBackTimeAbsList.remove(abs);
        }

    }

    @Override
    public void notifyObserver(int startTime, int endTime, int recType, int index, int end) {

        for (SearchRecordTimeAbs abs : mSearchBackTimeAbsList) {
            abs.onSearchRecordTimeCallBack(startTime, endTime, recType, index, end);
        }


    }
}
