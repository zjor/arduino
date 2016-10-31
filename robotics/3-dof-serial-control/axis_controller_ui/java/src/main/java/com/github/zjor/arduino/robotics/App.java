package com.github.zjor.arduino.robotics;

import processing.core.PApplet;

public class App extends PApplet {

    @Override
    public void settings() {
        size(800, 600);
    }

    @Override
    public void setup() {
        background(0);
    }

    @Override
    public void draw() {
    }

    public static void main(String[] args) {
        PApplet.main(App.class);
    }
}
