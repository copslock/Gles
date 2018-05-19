package com.app.jagles.activity.dialog;

import android.app.Activity;
import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.view.Display;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import com.app.jagles.R;

/**
 * Created by zasko on 2018/1/11.
 */

public class AddDeviceDialog extends Dialog {

    private static final String TAG = "AddDeviceDialog";


    private Context mContext;
    public TextView mTitleTv;
    public EditText mNameEdt;
    public EditText mPWDEdt;
    public EditText mNumberEdt;
    public Button mAddBtn;


    public AddDeviceDialog(@NonNull Context context) {
        super(context, R.style.dialog_alert_base);
        mContext = context;

    }


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.dialog_add_device_layout);

        initView();
    }

    private void initView() {

        mTitleTv = (TextView) findViewById(R.id.title_tv);
        mNameEdt = (EditText) findViewById(R.id.name_edt);
        mPWDEdt = (EditText) findViewById(R.id.pwd_edt);
        mNumberEdt = (EditText) findViewById(R.id.number_edt);
        mAddBtn = (Button) findViewById(R.id.add_btn);
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
