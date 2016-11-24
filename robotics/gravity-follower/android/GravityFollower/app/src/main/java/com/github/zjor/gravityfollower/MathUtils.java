package com.github.zjor.gravityfollower;

public class MathUtils {

    public static int map(float value, float minIn, float maxIn, int minOut, int maxOut) {
        float x1 = minIn, x2 = maxIn;
        float y1 = minOut, y2 = maxOut;

        float a = (y2 - y1) / (x2 - x1);
        float b = (y1 * x2 - y2 * x1) / (x2 - x1);

        value = Math.max(value, minIn);
        value = Math.min(value, maxIn);

        return (int) (a * value + b);
    }

}
