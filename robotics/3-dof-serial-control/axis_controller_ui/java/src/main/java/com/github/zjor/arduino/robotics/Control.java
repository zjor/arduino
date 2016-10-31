package com.github.zjor.arduino.robotics;

import lombok.AllArgsConstructor;
import lombok.Data;
import processing.core.PApplet;

@Data
@AllArgsConstructor
public class Control {

    private int keyCode;
    private boolean pressed;
    private Action action;

    public void keyPressed(int code) {
        if (keyCode == code) {
            pressed = true;
            action.apply();
        }
    }

    public void keyReleased(int code) {
        if (keyCode == code) {
            pressed = false;
        }
    }

    public void draw(PApplet applet, float x, float y, float w, float h) {
        applet.fill(pressed ? 255 : 0);
        applet.rect(x, y, w, h);
        applet.fill(pressed ? 0 : 255);
        applet.text(getText(), x + w / 2, y + h / 2);
    }

    private String getText() {
        switch (keyCode) {
            case PApplet.LEFT:
                return "Left";
            case PApplet.RIGHT:
                return "Right";
            default:
                return "" + PApplet.parseChar(keyCode);
        }
    }

}
