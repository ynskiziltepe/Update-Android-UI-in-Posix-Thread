package com.yunus.androiduiinposixthread;

import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;


public class MainActivity extends AppCompatActivity {

    // Used to load the 'pthreadandroidui' library on application startup.
    static {
        System.loadLibrary("pthreadandroidui");
    }

    public enum UI_OPERATION_TYPE
    {
        UI_SET_TEXT,
        UI_CLEAR_TEXT
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

    }

    void textFromJniThread(String msg) {
            // Set text
            ((TextView)findViewById(R.id.textView)).setText(msg);
    }

    public void onClicked(View view) {

        if(R.id.btnTextFromPthread == view.getId()){
            // Call JNI for set ui text in Posix Thread
            updateAndroidUIinPosixThread(UI_OPERATION_TYPE.UI_SET_TEXT.ordinal());
        }
        else if(R.id.btnClearTextInPthread == view.getId()) {
            // Call JNI for clear ui text in posix thread
            updateAndroidUIinPosixThread(UI_OPERATION_TYPE.UI_CLEAR_TEXT.ordinal());
        }
    }

    public native void updateAndroidUIinPosixThread(int opType);
}