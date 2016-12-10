package com.github.zjor.facetracker;

import android.content.Context;
import android.hardware.Camera;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.OrientationEventListener;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.FrameLayout;

import java.util.List;

public class MainActivity extends AppCompatActivity implements SurfaceHolder.Callback{

    public static final String TAG = MainActivity.class.getSimpleName();

    private Camera mCamera;
    private SurfaceView mView;
    private FaceDetectorView mOverlay;

    private int mOrientation;
    private int mOrientationCompensation;
    private OrientationEventListener mOrientationEventListener;

    private int mDisplayRotation;
    private int mDisplayOrientation;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        mView = new SurfaceView(this);
        setContentView(mView);

        mOverlay = new FaceDetectorView(this);
        addContentView(mOverlay, new FrameLayout.LayoutParams(FrameLayout.LayoutParams.MATCH_PARENT, FrameLayout.LayoutParams.MATCH_PARENT));

        mOrientationEventListener = new SimpleOrientationEventListener(this);
        mOrientationEventListener.enable();

    }

    @Override
    protected void onPostCreate(Bundle savedInstanceState) {
        super.onPostCreate(savedInstanceState);
        SurfaceHolder holder = mView.getHolder();
        holder.addCallback(this);
    }


    @Override
    protected void onPause() {
        super.onPause();
        mOrientationEventListener.disable();
    }

    @Override
    protected void onResume() {
        super.onResume();
        mOrientationEventListener.enable();
    }

    private Camera.FaceDetectionListener faceDetectionListener = new Camera.FaceDetectionListener() {
        @Override
        public void onFaceDetection(Camera.Face[] faces, Camera camera) {
            mOverlay.setFaces(faces);
        }
    };

    @Override
    public void surfaceCreated(SurfaceHolder surfaceHolder) {
        mCamera = Camera.open();
        mCamera.setFaceDetectionListener(faceDetectionListener);
        mCamera.startFaceDetection();
        try {
            mCamera.setPreviewDisplay(surfaceHolder);
        } catch (Exception e) {
            Log.e(TAG, "Could not preview the image.", e);
        }
    }

    @Override
    public void surfaceChanged(SurfaceHolder surfaceHolder, int i, int i1, int i2) {
        if (surfaceHolder.getSurface() == null) {
            return;
        }
        // Try to stop the current preview:
        try {
            mCamera.stopPreview();
        } catch (Exception e) {
            // Ignore...
        }
        // Get the supported preview sizes:
        Camera.Parameters parameters = mCamera.getParameters();
        List<Camera.Size> previewSizes = parameters.getSupportedPreviewSizes();
        Camera.Size previewSize = previewSizes.get(0);
        // And set them:
        parameters.setPreviewSize(previewSize.width, previewSize.height);
        mCamera.setParameters(parameters);
        // Now set the display orientation for the camera. Can we do this differently?
        mDisplayRotation = Utils.getDisplayRotation(this);
        mDisplayOrientation = Utils.getDisplayOrientation(mDisplayRotation, 0);
        mCamera.setDisplayOrientation(mDisplayOrientation);

        if (mOverlay != null) {
            mOverlay.setDisplayOrientation(mDisplayOrientation);
        }

        mCamera.startPreview();
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder surfaceHolder) {
        mCamera.setPreviewCallback(null);
        mCamera.setFaceDetectionListener(null);
        mCamera.setErrorCallback(null);
        mCamera.release();
        mCamera = null;
    }

    private class SimpleOrientationEventListener extends OrientationEventListener {

        public SimpleOrientationEventListener(Context context) {
            super(context, SensorManager.SENSOR_DELAY_NORMAL);
        }

        @Override
        public void onOrientationChanged(int orientation) {
            // We keep the last known orientation. So if the user first orient
            // the camera then point the camera to floor or sky, we still have
            // the correct orientation.
            if (orientation == ORIENTATION_UNKNOWN) return;
            mOrientation = Utils.roundOrientation(orientation, mOrientation);
            // When the screen is unlocked, display rotation may change. Always
            // calculate the up-to-date orientationCompensation.
            int orientationCompensation = mOrientation
                    + Utils.getDisplayRotation(MainActivity.this);
            if (mOrientationCompensation != orientationCompensation) {
                mOrientationCompensation = orientationCompensation;
                mOverlay.setOrientation(mOrientationCompensation);
            }
        }
    }


}
