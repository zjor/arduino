package com.github.zjor.gravityfollower;

import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbManager;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.SeekBar;
import android.widget.Switch;
import android.widget.TextView;

import com.felhr.usbserial.UsbSerialDevice;
import com.felhr.usbserial.UsbSerialInterface;

import java.io.UnsupportedEncodingException;

public class MainActivity extends AppCompatActivity implements SensorEventListener {

    private static final String ACTION_USB_PERMISSION = "com.android.example.USB_PERMISSION";

    private Switch ledSwitch;
    private TextView deviceStatusTextView;

    private TextView servoXValueTextView;
    private TextView servoYValueTextView;
    private TextView servoZValueTextView;

    private TextView gravityXTextView;
    private TextView gravityYTextView;
    private TextView gravityZTextView;

    private TextView receivedTextView;

    private Button refreshButton;

    private SensorManager sensorManager;
    private Sensor sensor;

    private UsbManager usbManager;
    private UsbDevice device;

    private ArduinoDevice arduinoDevice;

    private SeekBar.OnSeekBarChangeListener servoChangeListener = new SeekBar.OnSeekBarChangeListener() {
        @Override
        public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
            TextView textView;
            int servoIndex;

            switch (seekBar.getId()) {
                case R.id.seekBar1:
                    textView = servoXValueTextView;
                    servoIndex = 0;
                    break;
                case R.id.seekBar2:
                    textView = servoYValueTextView;
                    servoIndex = 1;
                    break;
                case R.id.seekBar3:
                    textView = servoZValueTextView;
                    servoIndex = 2;
                    break;
                default:
                    throw new IllegalArgumentException("Unknown ID");
            }
            textView.setText("" + progress);
            if (arduinoDevice != null && arduinoDevice.isOpened()) {
                arduinoDevice.servo(servoIndex, progress);
            }
        }

        @Override
        public void onStartTrackingTouch(SeekBar seekBar) {

        }

        @Override
        public void onStopTrackingTouch(SeekBar seekBar) {

        }
    };

    private UsbSerialInterface.UsbReadCallback usbReadCallback = new UsbSerialInterface.UsbReadCallback() {
        @Override
        public void onReceivedData(final byte[] bytes) {
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    try {
                        receivedTextView.setText(receivedTextView.getText() + new String(bytes, "UTF-8"));
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
                            deviceStatusTextView.setText("Unable to open arduino device");
                        } else {
                            deviceStatusTextView.setText("Connected");
                        }
                    } catch (Throwable t) {
                        deviceStatusTextView.setText("Failed to open connection: " + t.getMessage());
                    }
                }
            }
        }
    };


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        sensorManager = (SensorManager) getSystemService(SENSOR_SERVICE);
        sensor = sensorManager.getDefaultSensor(Sensor.TYPE_GRAVITY);

        usbManager = (UsbManager) getSystemService(Context.USB_SERVICE);

        resolveViews();

        ((SeekBar) findViewById(R.id.seekBar1)).setOnSeekBarChangeListener(servoChangeListener);
        ((SeekBar) findViewById(R.id.seekBar2)).setOnSeekBarChangeListener(servoChangeListener);
        ((SeekBar) findViewById(R.id.seekBar3)).setOnSeekBarChangeListener(servoChangeListener);

        refreshButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                initArduinoDevice();
            }
        });

        ledSwitch.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (arduinoDevice != null && arduinoDevice.isOpened()) {
                    arduinoDevice.led(isChecked);
                }
            }
        });

    }

    private void resolveViews() {
        servoXValueTextView = (TextView) findViewById(R.id.value1);
        servoYValueTextView = (TextView) findViewById(R.id.value2);
        servoZValueTextView = (TextView) findViewById(R.id.value3);

        gravityXTextView = (TextView) findViewById(R.id.gravityX);
        gravityYTextView = (TextView) findViewById(R.id.gravityY);
        gravityZTextView = (TextView) findViewById(R.id.gravityZ);

        refreshButton = (Button) findViewById(R.id.refreshDevice);
        ledSwitch = (Switch) findViewById(R.id.ledSwitch);
        deviceStatusTextView = (TextView) findViewById(R.id.deviceStatus);

        receivedTextView = (TextView) findViewById(R.id.received);
    }

    @Override
    protected void onResume() {
        super.onResume();
        sensorManager.registerListener(this, sensor, SensorManager.SENSOR_DELAY_UI);
        IntentFilter filter = new IntentFilter();
        filter.addAction(ACTION_USB_PERMISSION);
        registerReceiver(broadcastReceiver, filter);
        initArduinoDevice();
    }

    @Override
    protected void onPause() {
        super.onPause();
        sensorManager.unregisterListener(this);
        unregisterReceiver(broadcastReceiver);
        if (arduinoDevice != null) {
            arduinoDevice.close();
            refreshButton.setEnabled(true);
        }
    }

    @Override
    public void onSensorChanged(SensorEvent sensorEvent) {
        gravityXTextView.setText("" + sensorEvent.values[0]);
        gravityYTextView.setText("" + sensorEvent.values[1]);
        gravityZTextView.setText("" + sensorEvent.values[2]);
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int i) {

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
            deviceStatusTextView.setText("Arduino detected. Requesting permission...");
        } else {
            deviceStatusTextView.setText("Not connected");
        }
    }
}
