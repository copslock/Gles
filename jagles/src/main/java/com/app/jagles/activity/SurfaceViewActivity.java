package com.app.jagles.activity;

import android.media.MediaPlayer;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import com.app.jagles.R;
import com.app.jagles.activity.base.BaseActivity;

import butterknife.BindView;
import butterknife.ButterKnife;


/**
 * Created by zasko on 2018/1/16.
 */

public class SurfaceViewActivity extends BaseActivity implements SurfaceHolder.Callback, MediaPlayer.OnCompletionListener, MediaPlayer.OnErrorListener, MediaPlayer.OnInfoListener, MediaPlayer.OnPreparedListener, MediaPlayer.OnSeekCompleteListener, MediaPlayer.OnVideoSizeChangedListener {


//@BindView(R.id.surface_view)
SurfaceView mSurfaceView;

    private SurfaceHolder mHolder;
    private MediaPlayer mMediaPlayer;


    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_surface_view);
        ButterKnife.bind(this);

        initData();

    }

    private void initData() {
        mHolder = mSurfaceView.getHolder();
        mHolder.addCallback(this);

        mMediaPlayer = new MediaPlayer();

        mMediaPlayer.setOnCompletionListener(this); /*网络流媒体播放结束监听*/
        mMediaPlayer.setOnErrorListener(this); /*设置错误信息监听*/
        mMediaPlayer.setOnInfoListener(this); /**/
        mMediaPlayer.setOnPreparedListener(this);/**/
        mMediaPlayer.setOnSeekCompleteListener(this);/**/
        mMediaPlayer.setOnVideoSizeChangedListener(this);/**/

    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {

    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {

    }

    @Override
    public void onCompletion(MediaPlayer mp) {

    }

    @Override
    public boolean onError(MediaPlayer mp, int what, int extra) {
        return false;
    }

    @Override
    public boolean onInfo(MediaPlayer mp, int what, int extra) {
        return false;
    }

    @Override
    public void onPrepared(MediaPlayer mp) {

    }

    @Override
    public void onSeekComplete(MediaPlayer mp) {

    }

    @Override
    public void onVideoSizeChanged(MediaPlayer mp, int width, int height) {

    }
}
