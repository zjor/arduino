package com.github.zjor.arduino.robotics;

import lombok.Getter;

import java.util.LinkedList;
import java.util.List;

public class AxisControl {

    public static final int MIN_THRESHOLD = 0;
    public static final int MAX_THRESHOLD = 180;

    private PortHolder portHolder;

    @Getter
    private int index;

    @Getter
    private int value;

    @Getter
    private Control inc, dec;

    private List<Control> controls = new LinkedList<>();

    public AxisControl(PortHolder portHolder, int index, int initValue, int incKeyCode, int decKeyCode) {
        this.portHolder = portHolder;
        this.index = index;
        this.value = initValue;
        inc = new Control(incKeyCode, false, () -> setValue(Math.min(MAX_THRESHOLD, value + 1)));
        dec = new Control(decKeyCode, false, () -> setValue(Math.max(MIN_THRESHOLD, value - 1)));
        controls.add(inc);
        controls.add(dec);
    }

    public void keyPressed(int code) {
        controls.stream().forEach(c -> c.keyPressed(code));
    }

    public void keyReleased(int code) {
        controls.stream().forEach(c -> c.keyReleased(code));
    }

    private void setValue(int newValue) {
        if (value != newValue) {
            value = newValue;
            portHolder.getPort().ifPresent(p -> p.write(index + ":" + value + "$"));
        }
    }

}
