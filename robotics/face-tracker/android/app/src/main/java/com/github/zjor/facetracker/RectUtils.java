package com.github.zjor.facetracker;

import android.graphics.Rect;
import android.graphics.RectF;

public class RectUtils {

    public static Rect convert(RectF r) {
        return new Rect((int)r.left, (int)r.top, (int)r.right, (int)r.bottom);
    }

    public static RectF convert(Rect r) {
        return new RectF(r.left, r.top, r.right, r.bottom);
    }

}
