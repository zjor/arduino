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
import android.widget.ToggleButton;

import com.felhr.usbserial.UsbSerialDevice;
import com.felhr.usbserial.UsbSerialInterface;

import java.io.UnsupportedEncodingException;
import java.util.concurrent.TimeUnit;

import rx.Observable;
import rx.android.schedulers.AndroidSchedulers;
import rx.functions.Action1;
import rx.functions.Func1;
import rx.functions.Func2;
import rx.schedulers.Schedulers;
import rx.subjects.BehaviorSubject;
import rx.subjects.Subject;
import rx.subscriptions.CompositeSubscription;

public class MainActivity extends AppCompatActivity implements SensorEventListener {

    private static final String ACTION_USB_PERMISSION = "com.android.example.USB_PERMISSION";

    private static final float MAX_SENSOR_VALUE = 9.5f; //9.81f;
    private static final int WINDOW_SIZE = 3;

    private Switch ledSwitch;
    private TextView deviceStatusTextView;

    private TextView servoXValueTextView, servoYValueTextView, servoZValueTextView;
    private TextView gravityXTextView, gravityYTextView, gravityZTextView;
    private SeekBar seekBarX, seekBarY, seekBarZ;
    private boolean[] isManual = new boolean[] {false, false, false};

    private TextView receivedTextView;

    private Button refreshButton;

    private ToggleButton modeToggleButton;

    private SensorManager sensorManager;
    private Sensor sensor;

    private UsbManager usbManager;
    private UsbDevice device;

    private ArduinoDevice arduinoDevice;

    private Subject<ServoCommand, ServoCommand> servoStream = BehaviorSubject.create();
    private Subject<SensorEvent, SensorEvent> sensorStream = BehaviorSubject.create();
    private CompositeSubscription subscriptions = new CompositeSubscription();

    private SeekBar.OnSeekBarChangeListener servoChangeListener = new SeekBar.OnSeekBarChangeListener() {
        @Override
        public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
            TextView textView;

            switch (seekBar.getId()) {
                case R.id.seekBar1:
                    textView = servoXValueTextView;
                    if (fromUser) isManual[0] = true;
                    break;
                case R.id.seekBar2:
                    textView = servoYValueTextView;
                    if (fromUser) isManual[1] = true;
                    break;
                case R.id.seekBar3:
                    textView = servoZValueTextView;
                    if (fromUser) isManual[2] = true;
                    break;
                default:
                    throw new IllegalArgumentException("Unknown ID");
            }
            textView.setText("" + progress);
            servoStream.onNext(new ServoCommand(new int[]{seekBarX.getProgress(), seekBarY.getProgress(), seekBarZ.getProgress()}));
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
                        StringBuilder text = new StringBuilder(receivedTextView.getText());
                        text.insert(0, new String(bytes, "UTF-8"));
                        text.setLength(1024);
                        receivedTextView.setText(text);
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

        seekBarX.setOnSeekBarChangeListener(servoChangeListener);
        seekBarY.setOnSeekBarChangeListener(servoChangeListener);
        seekBarZ.setOnSeekBarChangeListener(servoChangeListener);

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

        modeToggleButton.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                isManual = new boolean[] {false, false, false};
            }
        });

        subscriptions.add(servoStream
                .map(new Func1<ServoCommand, int[]>() {
                    @Override
                    public int[] call(ServoCommand c) {
                        return c.values;
                    }
                })
                .window(WINDOW_SIZE)
                .flatMap(new Func1<Observable<int[]>, Observable<ServoCommand>>() {
                    @Override
                    public Observable<ServoCommand> call(Observable<int[]> o) {
                        return o.reduce(new Func2<int[], int[], int[]>() {
                            @Override
                            public int[] call(int[] i1, int[] i2) {
                                int[] i3 = new int[i1.length];
                                for (int i = 0; i < i1.length; i++) {
                                    i3[i] = i1[i] + i2[i];
                                }
                                return i3;
                            }
                        }).map(new Func1<int[], ServoCommand>() {
                            @Override
                            public ServoCommand call(int[] ints) {
                                int[] r = new int[ints.length];
                                for (int i = 0; i < 3; i++) {
                                    r[i] = ints[i] / WINDOW_SIZE;
                                }
                                return new ServoCommand(r);
                            }
                        });
                    }
                })
                .subscribeOn(Schedulers.io())
                .observeOn(AndroidSchedulers.mainThread())
                .subscribe(new Action1<ServoCommand>() {
                    @Override
                    public void call(ServoCommand cmd) {
                        if (arduinoDevice != null && arduinoDevice.isOpened()) {
                            for (int i = 0; i < 3; i++) {
                                arduinoDevice.servo(i, cmd.values[i]);
                            }
                        }
                    }
                }));

        subscriptions.add(sensorStream
                .throttleLast(50, TimeUnit.MILLISECONDS)
                .observeOn(AndroidSchedulers.mainThread())
                .subscribe(new Action1<SensorEvent>() {
                    @Override
                    public void call(SensorEvent event) {
                        gravityXTextView.setText("" + event.values[0]);
                        gravityYTextView.setText("" + event.values[1]);
                        gravityZTextView.setText("" + event.values[2]);
                        if (modeToggleButton.isChecked()) {
                            if (!isManual[0]) seekBarX.setProgress(MathUtils.map(event.values[0], -MAX_SENSOR_VALUE, MAX_SENSOR_VALUE, 0, 180));
                            if (!isManual[1]) seekBarY.setProgress(MathUtils.map(event.values[1], -MAX_SENSOR_VALUE, MAX_SENSOR_VALUE, 0, 180));
                            if (!isManual[2]) seekBarZ.setProgress(MathUtils.map(event.values[2], 0, MAX_SENSOR_VALUE, 0, 180));
                        }
                    }
                }));

    }

    private void resolveViews() {
        servoXValueTextView = (TextView) findViewById(R.id.value1);
        servoYValueTextView = (TextView) findViewById(R.id.value2);
        servoZValueTextView = (TextView) findViewById(R.id.value3);

        gravityXTextView = (TextView) findViewById(R.id.gravityX);
        gravityYTextView = (TextView) findViewById(R.id.gravityY);
        gravityZTextView = (TextView) findViewById(R.id.gravityZ);

        seekBarX = (SeekBar) findViewById(R.id.seekBar1);
        seekBarY = (SeekBar) findViewById(R.id.seekBar2);
        seekBarZ = (SeekBar) findViewById(R.id.seekBar3);

        refreshButton = (Button) findViewById(R.id.refreshDevice);
        ledSwitch = (Switch) findViewById(R.id.ledSwitch);
        deviceStatusTextView = (TextView) findViewById(R.id.deviceStatus);

        receivedTextView = (TextView) findViewById(R.id.received);

        modeToggleButton = (ToggleButton) findViewById(R.id.modeToggle);
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
    protected void onDestroy() {
        super.onDestroy();
        subscriptions.unsubscribe();
    }

    @Override
    public void onSensorChanged(SensorEvent sensorEvent) {
        sensorStream.onNext(sensorEvent);
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

    public static class ServoCommand {

        final int[] values;

        public ServoCommand(int[] values) {
            this.values = values;
        }
    }
}
