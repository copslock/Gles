package com.app.jagles.video;

import java.util.HashMap;
import java.util.List;

/**
 * Created by Administrator on 2016/10/19.
 */

public class ConnectManager {

    private HashMap<String, Long> connectInFos = null;


    private static ConnectManager m = new ConnectManager();

    private ConnectManager() {
        connectInFos = new HashMap<String, Long>();
    }

    public static ConnectManager getInstance() {
        return m;
    }

    public void addConnect(String msg, long handle) {
        connectInFos.put(msg, handle);

    }

    public void removeConnect(String msg) {
        connectInFos.remove(msg);
    }

    public boolean containConnect(String msg) {
        return connectInFos.containsKey(msg);
    }

    public int size() {
        return connectInFos.size();
    }

    public void removeAll() {
        connectInFos.clear();
        connectInFos = new HashMap<String, Long>();
    }

    public long getConnect(String msg) {
        if (connectInFos.get(msg) == null) {
            return 0;
        } else {
            return connectInFos.get(msg);
        }
    }

    public HashMap<String, Long> getConnectList() {
        return connectInFos;
    }


}
