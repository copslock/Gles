package com.app.jagles.activity.dialog;

import android.app.Activity;
import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.view.Display;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.TextView;

import com.app.jagles.R;

/**
 * Created by zasko on 2018/1/12.
 */

public class DeleteDialog extends Dialog {

    private Context mContext;

    public DeleteDialog(@NonNull Context context) {
        super(context, R.style.dialog_alert_base);
        mContext = context;
    }

    public TextView mContentTv;

    public Button mDeleteBtn;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.dialog_delete_layout);

        mContentTv = (TextView) findViewById(R.id.title_tv);
        mDeleteBtn = (Button) findViewById(R.id.delete_btn);


    }

    @Override
    protected void onStart() {
        super.onStart();
        WindowManager manager = ((Activity) mContext).getWindowManager();
        Display display = manager.getDefaultDisplay();
        WindowManager.LayoutParams params = this.getWindow().getAttributes();
        params.width = (int) ((display.getWidth()) * 0.8);
        this.getWindow().setAttributes(params);
    }
}
