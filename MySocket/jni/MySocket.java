package com;

import android.util.Log;

public class MySocket {

    String TAG = "MySocket";
    static {
        System.loadLibrary("mysocket");
    }

    public MySocket() {
        Log.i(TAG, "getVersion:" + getVersion());
    }

    public final static String SERVER_IP       = "192.168.0.113";
    public final static int SERVER_PORT        = 8990;
    private long nativeClient = 0;
    public boolean connectServer(String ip, int port) {
        nativeClient = ConnectServer(ip, port);
        Log.d(TAG, "ConnectServer:" + ip + ":" + port + " = " + nativeClient);
        return nativeClient != 0;
    }

    public boolean pull(String path) {
        Log.d(TAG, "pull:" + path);
        return 0 == PullFromServer(nativeClient, path);
    }

    public boolean sync(String path) {
        Log.d(TAG, "pull:" + path);
        return 0 == SyncFromServer(nativeClient, path);
    }

    public void dispose() {
        Log.d(TAG, "dispose nativeClient:" + nativeClient);
        if (0 != nativeClient) {
            DisconnectServer(nativeClient);
            nativeClient = 0;
        }
    }

    public boolean isConnected() { return 0 != nativeClient; }

    public static String getVersion() {
        return GetVersion();
    }
    private native static String GetVersion();

    private native long ConnectServer(String ip, int port);
    private native int DisconnectServer(long nativeClient);
    private native int PullFromServer(long nativeClient, String path);
    private native int SyncFromServer(long nativeClient, String path);
}
