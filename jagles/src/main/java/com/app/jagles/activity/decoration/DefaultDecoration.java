package com.app.jagles.activity.decoration;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.support.v7.widget.RecyclerView;
import android.util.Log;
import android.view.View;

import com.app.jagles.R;
import com.app.jagles.activity.utils.DisplayUtil;

/**
 * Created by zasko on 2018/1/10.
 */

public class DefaultDecoration extends RecyclerView.ItemDecoration {

    private static final String TAG = "DefaultDecoration";


    private Context mContext;

    private Paint mPaint;
    private int mPaddingLeft = 0;
    private int mPaddingRight = 0;

    private int mDividerHeight = 0;

    public DefaultDecoration(Context context) {
        init(context, 0, 0);
    }

    public DefaultDecoration(Context context, int paddingLeft, int paddingRight) {
        init(context, paddingLeft, paddingRight);
    }

    private void init(Context context, int paddingLeft, int paddingRight) {
        mContext = context;
        mPaddingLeft = paddingLeft;
        mPaddingRight = paddingRight;

        mDividerHeight = DisplayUtil.dp2px(mContext, 0.5f);

        Log.d(TAG, "init: ------->" + mDividerHeight);

        mPaint = new Paint();
        mPaint.setColor(mContext.getResources().getColor(R.color.colorPrimaryDark));
        mPaint.setAntiAlias(true);
//        mPaint.setStrokeWidth(f);

    }

    @Override
    public void onDraw(Canvas c, RecyclerView parent, RecyclerView.State state) {
        super.onDraw(c, parent, state);

        int childCount = parent.getChildCount();

        int left = parent.getPaddingLeft() + mPaddingLeft;
        int right = parent.getWidth() - mPaddingRight;
        for (int i = 0; i < childCount; i++) {
            View childView = parent.getChildAt(i);
            float top = childView.getBottom();
            float bottom = childView.getBottom() + mDividerHeight;
            c.drawLine(left, top, right, bottom, mPaint);
        }
    }

    @Override
    public void getItemOffsets(Rect outRect, View view, RecyclerView parent, RecyclerView.State state) {
        super.getItemOffsets(outRect, view, parent, state);
        outRect.bottom = mDividerHeight;

    }


}
