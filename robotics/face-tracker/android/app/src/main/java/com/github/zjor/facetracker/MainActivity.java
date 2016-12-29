package com.github.zjor.facetracker;

import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.RectF;
import android.hardware.Camera;
import android.hardware.SensorManager;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbManager;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.OrientationEventListener;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.FrameLayout;
import android.widget.Toast;

import com.felhr.usbserial.UsbSerialDevice;
import com.felhr.usbserial.UsbSerialInterface;

import java.io.UnsupportedEncodingException;
import java.util.List;

import rx.functions.Action1;
import rx.subscriptions.CompositeSubscription;

public class MainActivity extends AppCompatActivity implements SurfaceHolder.Callback {

    public static final String TAG = MainActivity.class.getSimpleName();

    private static final String ACTION_USB_PERMISSION = "com.android.example.USB_PERMISSION";

    private int cameraId = 0;
    private Camera mCamera;
    private SurfaceView mView;
    private FaceDetectorView mOverlay;

    private int mOrientation;
    private int mOrientationCompensation;
    private OrientationEventListener mOrientationEventListener;

    private int mDisplayRotation;
    private int mDisplayOrientation;

    private UsbManager usbManager;
    private UsbDevice device;

    private ArduinoDevice arduinoDevice;

    private CompositeSubscription subscriptions = new CompositeSubscription();

    private Position position = new Position();

    private UsbSerialInterface.UsbReadCallback usbReadCallback = new UsbSerialInterface.UsbReadCallback() {
        @Override
        public void onReceivedData(final byte[] bytes) {
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    try {
                        StringBuilder text = new StringBuilder();
                        text.insert(0, new String(bytes, "UTF-8"));
                        text.setLength(1024);
                        Toast.makeText(MainActivity.this, "Received: " + text, Toast.LENGTH_SHORT).show();
                    } catch (UnsupportedEncodingException e) {
                        Log.wtf(this.getClass().getSimpleName(), e);
                    }
                }
            });

        }
    };

    private final BroadcastReceiver broadcastReceiver = new BroadcastReceiver() {

        @Override
        public void onReceive(Context context, Intent intent) {
            if (intent.getAction().equals(ACTION_USB_PERMISSION)) {
                boolean granted = intent.getExtras().getBoolean(UsbManager.EXTRA_PERMISSION_GRANTED);
                if (granted) {
                    try {
                        UsbDeviceConnection connection = usbManager.openDevice(device);
                        arduinoDevice = new ArduinoDevice(UsbSerialDevice.createUsbSerialDevice(device, connection));
                        if (!arduinoDevice.open(usbReadCallback)) {
                            Toast.makeText(MainActivity.this, "Failed to open Arduino", Toast.LENGTH_SHORT).show();
                        } else {
                            Toast.makeText(MainActivity.this, "Connection established with Arduino", Toast.LENGTH_SHORT).show();
                        }
                    } catch (Throwable t) {
                        Toast.makeText(MainActivity.this, "Failed to open connection to Arduino", Toast.LENGTH_SHORT).show();
                    }
                }
            }
        }
    };


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        mView = new SurfaceView(this);
        setContentView(mView);

        mOverlay = new FaceDetectorView(this);
        addContentView(mOverlay, new FrameLayout.LayoutParams(FrameLayout.LayoutParams.MATCH_PARENT, FrameLayout.LayoutParams.MATCH_PARENT));

        mOrientationEventListener = new SimpleOrientationEventListener(this);
        mOrientationEventListener.enable();

        usbManager = (UsbManager) getSystemService(Context.USB_SERVICE);

        subscriptions.add(mOverlay.getFaceStream().subscribe(new Action1<RectF>() {
            @Override
            public void call(RectF rect) {

                int dx = (int) (rect.centerX() - mView.getHeight() / 2);
                int dy = (int) (rect.centerY() - mView.getWidth() / 2);

                boolean isDirty = false;

                if (Math.abs(dx) > 20) {
                    dx = dx / Math.abs(dx);
                    int x = position.getX() - dx;
                    position.setX(MathUtils.inRange(x, 5, 175));
                    isDirty = true;
                }

                if (Math.abs(dy) > 20) {
                    dy = dy / Math.abs(dy);
                    int y = position.getY() - dy;
                    position.setY(MathUtils.inRange(y, 5, 60));
                    isDirty = true;
                }

                if (isDirty) {
                    sendPosition();
                    Log.i(TAG, position.toString());
                }


            }
        }));

    }

    private void sendPosition() {
        if (arduinoDevice != null && arduinoDevice.isOpened()) {
            arduinoDevice.servo(1, position.getX());
            arduinoDevice.servo(2, position.getY());
        }
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

        unregisterReceiver(broadcastReceiver);
        if (arduinoDevice != null) {
            arduinoDevice.close();
        }

    }

    @Override
    protected void onResume() {
        super.onResume();
        mOrientationEventListener.enable();

        IntentFilter filter = new IntentFilter();
        filter.addAction(ACTION_USB_PERMISSION);
        registerReceiver(broadcastReceiver, filter);
        initArduinoDevice();

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        subscriptions.unsubscribe();
    }

    private Camera.FaceDetectionListener faceDetectionListener = new Camera.FaceDetectionListener() {
        @Override
        public void onFaceDetection(Camera.Face[] faces, Camera camera) {
            mOverlay.setFaces(faces);
        }
    };

    private Camera openFrontalCamera() {
        for (int i = 0; i < Camera.getNumberOfCameras(); i++) {
            Camera.CameraInfo info = new Camera.CameraInfo();
            Camera.getCameraInfo(i, info);
            if (info.facing == Camera.CameraInfo.CAMERA_FACING_FRONT) {
                cameraId = i;
                return Camera.open(i);
            }
        }
        return null;
    }

    @Override
    public void surfaceCreated(SurfaceHolder surfaceHolder) {
        mCamera = openFrontalCamera();
        if (mCamera == null) {
            throw new RuntimeException("Unable to open frontal camera");
        }
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
        mDisplayOrientation = Utils.getDisplayOrientation(mDisplayRotation, cameraId);
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


    private UsbDevice discoverArduinoDevice() {
        for (UsbDevice device : usbManager.getDeviceList().values()) {
            if (device.getVendorId() == 0x2341) {
                return device;
            }
        }
        return null;
    }

    private void initArduinoDevice() {
        if (arduinoDevice != null && arduinoDevice.isOpened()) {
            return;
        }

        device = discoverArduinoDevice();
        if (device != null) {
            PendingIntent pi = PendingIntent.getBroadcast(this, 0, new Intent(ACTION_USB_PERMISSION), 0);
            usbManager.requestPermission(device, pi);
            Toast.makeText(MainActivity.this, "Arduino detected. Requesting permissions...", Toast.LENGTH_SHORT).show();
        } else {
            Toast.makeText(MainActivity.this, "Arduino is not connected", Toast.LENGTH_SHORT).show();
        }
    }

    public static class ServoCommand {

        final int[] values;

        public ServoCommand(int[] values) {
            this.values = values;
        }
    }

}
