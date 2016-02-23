package com.kugou.openglcamera;

import android.app.Activity;
import android.graphics.PixelFormat;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.os.Environment;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.Window;
import android.view.WindowManager;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.Spinner;

public class MainActivity extends Activity {
    private CameraView mCameraView = null;
    private CameraHelper mCameraHelper = null;
    private CameraLoader mCamera = null;

    private String mSDCardPath = Environment.getExternalStorageDirectory().getPath();
    private String mFilePath = mSDCardPath + "/record.h264";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        getWindow().setFormat(PixelFormat.TRANSLUCENT);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);

        setContentView(R.layout.activity_main);

        GLSurfaceView surfaceView = (GLSurfaceView) findViewById(R.id.surface_camera);
//		int width = getWindowManager().getDefaultDisplay().getWidth();
//		surfaceView.setLayoutParams( new LayoutParams( width, width * 3 / 2 ) );

        mCameraView = new CameraView(this);
        mCameraView.setGLSurfaceView(surfaceView);

        mCameraHelper = new CameraHelper(this);
        int frontCameraId = mCameraHelper.hasFrontCamera();
        if (frontCameraId == -1) {
            frontCameraId = 0;
        }
        mCamera = new CameraLoader(frontCameraId, this, mCameraHelper, mCameraView);

        Button button = (Button) findViewById(R.id.switchcamera);
        button.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View arg0) {
                // TODO Auto-generated method stub
                mCamera.switchCamera();
            }
        });

        button = (Button) findViewById(R.id.gaussfilter);
        button.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View arg0) {
                if (mCameraView.getGaussFilterFlag()) {
                    ((Button) findViewById(R.id.gaussfilter)).setText("使用磨皮");
                    mCameraView.setGaussFilterFlag(false);
                } else {
                    ((Button) findViewById(R.id.gaussfilter)).setText("取消磨皮");
                    mCameraView.setGaussFilterFlag(true);
                }
            }
        });

        button = (Button) findViewById(R.id.startrecord);
        button.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View arg0) {
                mCameraView.startRecord(mFilePath);
            }
        });

        button = (Button) findViewById(R.id.stoprecord);
        button.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View arg0) {
                mCameraView.stopRecord();
            }
        });

        Spinner changefilterSpinner = (Spinner) findViewById(R.id.changefilter);
        ArrayAdapter<CharSequence> changefilterAdapter = ArrayAdapter.createFromResource(this,
                R.array.filter_spinner_array, android.R.layout.simple_spinner_item);
        changefilterAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        changefilterSpinner.setAdapter(changefilterAdapter);
        changefilterSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            public void onItemSelected(AdapterView<?> parent, View view, int pos, long id) {
                mCameraView.changeFilterType(pos);
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {
                // TODO Auto-generated method stub
            }
        });
    }

    @Override
    protected void onResume() {
        super.onResume();
        mCamera.onResume();
    }

    @Override
    protected void onPause() {
        mCamera.onPause();
        super.onPause();
    }
}
