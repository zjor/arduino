package com.github.zjor.facetracker;

public class Position {

    public static final int DEFAULT_X = 90;
    public static final int DEFAULT_Y = 30;

    private int x = DEFAULT_X;
    private int y = DEFAULT_Y;

    public void reset() {
        x = DEFAULT_X;
        y = DEFAULT_Y;
    }


    public int getX() {
        return x;
    }

    public void setX(int x) {
        this.x = x;
    }

    public int getY() {
        return y;
    }

    public void setY(int y) {
        this.y = y;
    }

    @Override
    public String toString() {
        return "Position{" +
                "x=" + x +
                ", y=" + y +
                '}';
    }
}
