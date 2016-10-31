package com.github.zjor.arduino.robotics;

import processing.core.PApplet;
import processing.serial.Serial;

import java.util.LinkedList;
import java.util.List;
import java.util.Optional;

public class App extends PApplet implements PortHolder {

    public static final String PORT_NAME = "/dev/tty.usbmodemfa131";

    private List<AxisControl> axes = new LinkedList<>();

    {
        axes.add(new AxisControl(this, 0, 0, 'D', 'A'));
        axes.add(new AxisControl(this, 1, 0, 'W', 'S'));
        axes.add(new AxisControl(this, 2, 0, RIGHT, LEFT));
    }

    private Serial port;

    @Override
    public void settings() {
        size(800, 600);
    }

    @Override
    public void setup() {
        background(0);
        stroke(155);
        textAlign(LEFT, TOP);
        try {
            port = new Serial(this, PORT_NAME, 9600);
            text("Port ready", 0, 0);
        } catch (Exception e) {
            text("Unable to open port: " + e.getMessage(), 0, 0);
        }
        textSize(18);
        textAlign(CENTER, CENTER);
    }

    @Override
    public void draw() {
        float blockWidth = 90;
        float blockHeight = 40;

        axes.forEach(a -> {

            float x = (width - ((blockWidth + 10) * 3)) / 2 + a.getIndex() * (blockWidth + 5);
            float y = (height - blockHeight * 3) / 2;

            a.getInc().draw(this, x, y, blockWidth, blockHeight);

            fill(0);
            rect(x, y + blockHeight, blockWidth, blockHeight);
            fill(255);
            text("" + a.getValue(), x + blockWidth / 2, y + 3 * blockHeight / 2);

            a.getDec().draw(this, x, y + 2 * blockHeight, blockWidth, blockHeight);

        });

        getPort().ifPresent(p -> {
            if (p.available() > 0) {
                System.out.print(p.readString());
            }
        });
    }

    @Override
    public void keyPressed() {
        axes.forEach(a -> a.keyPressed(keyCode));
    }

    @Override
    public void keyReleased() {
        axes.forEach(a -> a.keyReleased(keyCode));
    }

    @Override
    public Optional<Serial> getPort() {
        return Optional.ofNullable(port);
    }

    public static void main(String[] args) {
        PApplet.main(App.class);
    }
}
