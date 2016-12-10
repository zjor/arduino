package com.github.zjor.facetracker;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.RectF;
import android.hardware.Camera;
import android.view.View;

public class FaceDetectorView extends View {

    private Paint paint;

    private Camera.Face[] faces;

    private int mDisplayOrientation;
    private int mOrientation;

    public FaceDetectorView(Context context) {
        super(context);
        init();
    }

    private void init() {
        paint = new Paint();
        paint.setAntiAlias(true);
        paint.setDither(true);
        paint.setColor(Color.GREEN);
        paint.setAlpha(128);
        paint.setStyle(Paint.Style.FILL_AND_STROKE);
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);

        if (faces != null && faces.length > 0) {
            Matrix matrix = new Matrix();
            Utils.prepareMatrix(matrix, false, mDisplayOrientation, getWidth(), getHeight());
            canvas.save();
            matrix.postRotate(mOrientation);
            canvas.rotate(-mOrientation);
            RectF rectF = new RectF();
            for (Camera.Face face : faces) {
                rectF.set(face.rect);
                matrix.mapRect(rectF);
                canvas.drawRect(rectF, paint);
            }
            canvas.restore();
        }

    }

    public void setOrientation(int orientation) {
        mOrientation = orientation;
    }

    public void setDisplayOrientation(int displayOrientation) {
        mDisplayOrientation = displayOrientation;
        invalidate();
    }

    public void setFaces(Camera.Face[] faces) {
        this.faces = faces;
        invalidate();
    }
}
