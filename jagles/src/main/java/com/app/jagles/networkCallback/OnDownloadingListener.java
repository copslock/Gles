package com.app.jagles.networkCallback;

/**
 * Created by Administrator on 2017/3/24.
 */

public interface OnDownloadingListener {
    public void OnDownloading(int flag, int fdsize, long pos, int end);
}
