package com.github.zjor.facetracker;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.DashPathEffect;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.Rect;
import android.graphics.RectF;
import android.hardware.Camera;
import android.view.View;

public class FaceDetectorView extends View {

    private static final int ARROW_SIZE = 24;
    private static final int ARROW_MARGIN = 24;
    private static final int TEXT_MARGIN = 14;

    private Paint paint, markupPaint, dashedPaint;

    private Camera.Face[] faces;

    private RectF mainFaceRect;
    private long lastDetected;

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
        paint.setStrokeWidth(2.0f);
        paint.setStyle(Paint.Style.STROKE);

        markupPaint = new Paint();
        markupPaint.setAntiAlias(true);
        markupPaint.setDither(true);
        markupPaint.setColor(Color.GREEN);
        markupPaint.setStyle(Paint.Style.FILL_AND_STROKE);
        markupPaint.setTextSize(24);

        dashedPaint = new Paint();
        dashedPaint.setAntiAlias(true);
        dashedPaint.setDither(true);
        dashedPaint.setColor(Color.GREEN);
        dashedPaint.setStyle(Paint.Style.STROKE);
        dashedPaint.setPathEffect(new DashPathEffect(new float[]{10, 20,}, 0));

    }

    private RectF translateRect(RectF r) {
        Matrix matrix = new Matrix();
        Utils.prepareMatrix(matrix, false, mDisplayOrientation, getWidth(), getHeight());
        matrix.postRotate(mOrientation);
        matrix.mapRect(r);
        return r;
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);

        if (mainFaceRect != null) {
            canvas.save();
            canvas.rotate(-mOrientation);
            canvas.drawRect(mainFaceRect, paint);
            drawArrows(canvas, mainFaceRect);

            canvas.restore();
        }

    }

    private Camera.Face getBiggestFace(Camera.Face[] faces) {
        if (faces != null) {
            float s = .0f;
            Camera.Face mainFace = null;
            for (Camera.Face f : faces) {
                float ts = f.rect.width() * f.rect.height();
                if (ts > s) {
                    s = ts;
                    mainFace = f;
                }
            }
            return mainFace;
        } else {
            return null;
        }
    }


    private void drawArrows(Canvas canvas, RectF rect) {
        int arrowX, arrowTipX;
        float lineStartX;
        int deltaXRef;

        if (rect.centerX() < getWidth() / 2) {
            arrowX = getWidth() - ARROW_MARGIN;
            arrowTipX = arrowX + ARROW_SIZE / 2;
            lineStartX = rect.right;
            deltaXRef = 1;
        } else {
            arrowX = ARROW_MARGIN;
            arrowTipX = arrowX - ARROW_SIZE / 2;
            lineStartX = rect.left;
            deltaXRef = 0;
        }

        Path path = new Path();
        path.moveTo(arrowX, rect.centerY() - ARROW_SIZE / 2);
        path.lineTo(arrowX, rect.centerY() + ARROW_SIZE / 2);
        path.lineTo(arrowTipX, rect.centerY());
        path.close();
        canvas.drawPath(path, markupPaint);
        canvas.drawLine(lineStartX, rect.centerY(), arrowX, rect.centerY(), dashedPaint);

        int arrowY, arrowTipY;
        float lineStartY;
        int deltaYRef;

        if (rect.centerY() < getHeight() / 2) {
            arrowY = getHeight() - ARROW_MARGIN;
            arrowTipY = arrowY + ARROW_SIZE / 2;
            lineStartY = rect.bottom;
            deltaYRef = 1;
        } else {
            arrowY = ARROW_MARGIN;
            arrowTipY = arrowY - ARROW_SIZE / 2;
            lineStartY = rect.top;
            deltaYRef = 0;
        }

        path = new Path();
        path.moveTo(rect.centerX() - ARROW_SIZE / 2, arrowY);
        path.lineTo(rect.centerX() + ARROW_SIZE / 2, arrowY);
        path.lineTo(rect.centerX(), arrowTipY);
        path.close();
        canvas.drawPath(path, markupPaint);
        canvas.drawLine(rect.centerX(), lineStartY, rect.centerX(), arrowY, dashedPaint);

        String deltaX = "" + (int) (getWidth() / 2 - rect.centerX());
        Rect bounds = new Rect();
        markupPaint.getTextBounds(deltaX, 0, deltaX.length(), bounds);

        canvas.drawText(deltaX, arrowX + TEXT_MARGIN - (bounds.width() + TEXT_MARGIN * 2) * deltaXRef, rect.centerY() - TEXT_MARGIN, markupPaint);

        String deltaY = "" + (int) (getHeight() / 2 - rect.centerY());
        bounds = new Rect();
        markupPaint.getTextBounds(deltaY, 0, deltaY.length(), bounds);

        canvas.drawText(deltaY, rect.centerX() + TEXT_MARGIN, arrowY + TEXT_MARGIN - (bounds.height() + TEXT_MARGIN * 2) * deltaYRef, markupPaint);

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
        Camera.Face bigFace = getBiggestFace(faces);
        if (bigFace != null) {
            mainFaceRect = new RectF();
            mainFaceRect.set(bigFace.rect);
            translateRect(mainFaceRect);
            lastDetected = System.currentTimeMillis();
            invalidate();
        } else if (System.currentTimeMillis() - lastDetected > 2000) {
            mainFaceRect = null;
            invalidate();
        }
    }
}
