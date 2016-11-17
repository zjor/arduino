package com.github.zjor.gravityfollower;

import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.widget.SeekBar;
import android.widget.Switch;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity implements SensorEventListener {

    private Switch ledSwitch;
    private TextView deviceStatusTextView;

    private TextView servoXValueTextView;
    private TextView servoYValueTextView;
    private TextView servoZValueTextView;

    private TextView gravityXTextView;
    private TextView gravityYTextView;
    private TextView gravityZTextView;

    private SensorManager sensorManager;
    private Sensor sensor;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        sensorManager = (SensorManager) getSystemService(SENSOR_SERVICE);
        sensor = sensorManager.getDefaultSensor(Sensor.TYPE_GRAVITY);


        resolveViews();

        ((SeekBar) findViewById(R.id.seekBar1)).setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                servoXValueTextView.setText("" + i);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

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
    }

    @Override
    protected void onResume() {
        super.onResume();
        sensorManager.registerListener(this, sensor, SensorManager.SENSOR_DELAY_UI);
    }

    @Override
    protected void onPause() {
        super.onPause();
        sensorManager.unregisterListener(this);
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
}
