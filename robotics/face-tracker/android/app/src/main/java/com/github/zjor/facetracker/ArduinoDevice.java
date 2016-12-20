package com.github.zjor.facetracker;

import android.util.Log;

import com.felhr.usbserial.UsbSerialDevice;
import com.felhr.usbserial.UsbSerialInterface;

import java.io.UnsupportedEncodingException;

public class ArduinoDevice {

    private UsbSerialDevice device;
    private boolean opened;

    public ArduinoDevice(UsbSerialDevice device) {
        this.device = device;
    }

    public boolean open(UsbSerialInterface.UsbReadCallback readCallback) {
        if (opened = device.open()) {
            device.setBaudRate(9600);
            device.setDataBits(UsbSerialInterface.DATA_BITS_8);
            device.setStopBits(UsbSerialInterface.STOP_BITS_1);
            device.setParity(UsbSerialInterface.PARITY_NONE);
            device.setFlowControl(UsbSerialInterface.FLOW_CONTROL_OFF);
            device.read(readCallback);
            return true;
        }
        return false;
    }

    public void led(boolean enabled) {
        send("#L:" + (enabled ? '1' : '0') + '$');
    }

    public void servo(int index, int value) {
        if (value < 0 || value > 180) {
            throw new IllegalArgumentException("Value should be between 0 and 180");
        }
        send("#S:" + index + ':' + value + '$');

    }

    private void send(String data) {
        if (!opened) {
            throw new IllegalStateException("Arduino device is not opened");
        }

        try {
            device.write(data.getBytes("UTF-8"));
        } catch (UnsupportedEncodingException e) {
            Log.wtf(getClass().getSimpleName(), e);
        }
    }

    public void close() {
        if (!opened) {
            device.close();
            opened = false;
        }
    }

    public boolean isOpened() {
        return opened;
    }
}
