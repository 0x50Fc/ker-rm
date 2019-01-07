package cn.kkmofang.ker;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.ColorFilter;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.PixelFormat;
import android.graphics.PorterDuff;
import android.graphics.PorterDuffXfermode;
import android.graphics.Rect;
import android.graphics.RectF;
import android.graphics.Region;

/**
 * Created by zhanghailong on 2018/12/18.
 */

public class KerCanvas extends Object implements Image {

    protected final Canvas _canvas;
    protected final Bitmap _bitmap;
    protected final Paint _paint = new Paint(Paint.ANTI_ALIAS_FLAG);
    protected final Path _path = new Path();

    protected int _strokeWidth = 5;
    protected float _textSize;
    protected int _textColor;

    public KerCanvas(int width, int height) {
        super();
        _bitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
        _canvas = new Canvas(_bitmap);
        _canvas.drawColor(Color.TRANSPARENT, PorterDuff.Mode.CLEAR);
    }

    public KerCanvas(Canvas canvas, int width, int height) {
        super();
        _bitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
        _canvas = canvas;
        _canvas.drawColor(Color.TRANSPARENT, PorterDuff.Mode.CLEAR);
    }

    public int getWidth() {
        return _canvas.getWidth();
    }

    public int getHeight() {
        return _canvas.getHeight();
    }

    public void clearRect(int x, int y, int width, int height) {
        Paint p = new Paint(Paint.ANTI_ALIAS_FLAG);
        p.setXfermode(new PorterDuffXfermode(PorterDuff.Mode.CLEAR));
        _canvas.drawRect(new Rect(x, y, x + width, y + height), p);
    }

    public void clear() {
        Paint p = new Paint(Paint.ANTI_ALIAS_FLAG);
        p.setXfermode(new PorterDuffXfermode(PorterDuff.Mode.CLEAR));
        _canvas.drawPaint(p);
    }

    public int save() {
        return _canvas.save();
    }

    public void restore() {
        _canvas.restore();
    }

    public void rect(float x, float y, float width, float height) {
        _path.addRect(x, y, x + width, y + height, Path.Direction.CW);
    }

    public void fill() {
        _paint.setColor(_fillColor);
        _paint.setAlpha(0x0ff & (_fillColor >> 24));
        _paint.setStyle(Paint.Style.FILL);
        _canvas.drawPath(_path, _paint);
    }

    public void fillRect(float x, float y, float width, float height) {
        rect(x, y, width, height);
        fill();
    }

    public void stroke() {
        _paint.setStrokeWidth(_strokeWidth);
        _paint.setColor(_strokeColor);
        _paint.setAlpha(0x0ff & (_strokeColor >> 24));
        _paint.setStyle(Paint.Style.STROKE);
        _canvas.drawPath(_path, _paint);
    }

    public void strokeRect(float x, float y, float width, float height) {
        rect(x, y, width, height);
        stroke();
    }

    public void beginPath() {
        _path.reset();
    }

    public void closePath() {
        _path.close();
    }

    public void moveTo(float x, float y) {
        _path.moveTo(x, y);
    }

    public void lineTo(float x, float y) {
        _path.lineTo(x, y);
    }

    public void clip() {
        _canvas.clipPath(_path);
    }

    public void quadraticCurveTo(float cpx, float cpy, float x, float y) {
        _path.quadTo(cpx, cpy, x, y);
    }

    public void bezierCurveTo(float cp1x, float cp1y, float cp2x, float cp2y, float x, float y) {
        _path.cubicTo(cp1x, cp1y, cp2x, cp2y, x, y);
    }

    public void arc(float x, float y, float r, float sAngle, float eAngle, boolean counterclockwise) {
        int CLOCK_DIRECTION;
        if (counterclockwise) {
            CLOCK_DIRECTION = -1;
        } else {
            CLOCK_DIRECTION = 1;
        }
        sAngle = CLOCK_DIRECTION * sAngle * (float) (180.0 / Math.PI);
        eAngle = CLOCK_DIRECTION * eAngle * (float) (180.0 / Math.PI);
//        _path.addRect(new RectF(x - r, y - r, x + r, y + r), counterclockwise ? Path.Direction.CCW : Path.Direction.CW);

        _path.addArc(new RectF(x - r, y - r, x + r, y + r), sAngle, eAngle);
    }

    public void arcTo(float x1, float y1, float x2, float y2, float r) {
        //TODO 切线弧
        // arc()
    }

    /**
     * 矩形边缘不可以,矩形 内 可以
     * 损失精度
     *
     * @param x
     * @param y
     * @return
     */
    public boolean isPointInPath(float x, float y) {
        //TODO

        Region region = new Region();
        RectF bounds = new RectF();
        _path.computeBounds(bounds, true);
        // 第二个参数
        boolean b = region.setPath(_path, new Region((int) bounds.left, (int) bounds.top, (int) bounds.right, (int) bounds.bottom));

        //FIXME 损失精度
        boolean contains = region.contains((int) x, (int) y);
        return contains;
    }

    public void scale(float sx, float sy) {
        _canvas.scale(sx, sy);
    }

    public void rotate(float radians) {
        float degree = radians * (float) (180.0 / Math.PI);
        _canvas.rotate(degree);
    }

    public void translate(float x, float y) {
        _canvas.translate(x, y);
    }

    public void transform(float m11, float m12, float m21, float m22, float dx, float dy) {
        Matrix matrix = new Matrix();
        matrix.setValues(new float[]{m11, m21, dx, m12, m22, dy, 0, 0, 1});
        _canvas.concat(matrix);
    }

    public void setTransform(float m11, float m12, float m21, float m22, float dx, float dy) {
        Matrix matrix = new Matrix();
        matrix.setValues(new float[]{m11, m21, dx, m12, m22, dy, 0, 0, 1});
        _canvas.setMatrix(matrix);
    }

    public void fillText(String text, float x, float y) {
        fillText(text, x, y, -100);
    }

    public void fillText(String text, float x, float y, float maxWidth) {
        _paint.setStyle(Paint.Style.FILL);
        drawText(text, x, y, maxWidth);
    }

    public void strokeText(String text, float x, float y) {
        //FIXME 貌似不生效
        strokeText(text, x, y, -100);
    }

    public void strokeText(String text, float x, float y, float maxWidth) {
        _paint.setStyle(Paint.Style.STROKE);
        drawText(text, x, y, maxWidth);
    }


    public void drawText(String text, float x, float y, float maxWidth) {
        if (maxWidth < 0) {
            //TODO
        }
        _paint.setColor(_textColor);
        _paint.setTextSize(_textSize);

        _canvas.drawText(text, x, y, _paint);
    }

    public MeasureText measureText(String text) {
        float height = 0;
        float width = 0;
        Rect bounds = new Rect();
        _paint.getTextBounds(text, 0, text.length(), bounds);
        return new MeasureText(bounds.height(), bounds.width());
    }

    public static class MeasureText {
        public float height;
        public float width;

        public MeasureText() {
            this.height = 0;
            this.width = 0;
        }

        public MeasureText(float height, float width) {
            this.height = height;
            this.width = width;
        }
    }

    public void drawImage(Bitmap imgBitmap, float sx, float sy, float swidth, float sheight, float x, float y, float width, float height) {
        Rect rectSrc = new Rect((int) sx, (int) sy, (int) (sx + swidth), (int) (sy + sheight));
        Rect rectDst = new Rect((int) x, (int) y, (int) (x + width), (int) (y + height));
//        rectSrc = null;
        _canvas.drawBitmap(imgBitmap, rectSrc, rectDst, _paint);
    }

    public void drawImage(Bitmap imgBitmap, int sx, int sy, int swidth, int sheight, int x, int y, int width, int height) {
        Rect rectSrc = new Rect(sx, sy, sx + swidth, sy + sheight);
        Rect rectDst = new Rect(x, y, x + width, y + height);
//        rectSrc = null;
        _canvas.drawBitmap(imgBitmap, rectSrc, rectDst, _paint);
    }

    public void drawImage(Bitmap imgBitmap, float x, float y, float width, float height) {
        //TODO
        _canvas.drawBitmap(imgBitmap, x, y, _paint);
    }

    public void drawImage(Bitmap imgBitmap, float x, float y) {
        drawImage(imgBitmap, x, y, -100, -100);
    }

    public Bitmap createImageData(Bitmap imageData) {
        Bitmap copy = imageData.copy(Bitmap.Config.ARGB_8888, true);
        copy.eraseColor(0x00000000);
        return _bitmap.createBitmap(copy);
    }

    public Bitmap createImageData(int width, int height) {
        int length = width * height;
        int[] colors = new int[length];
        //FIXME 颜色 set 方法还需要调整
        for (int i = 0; i < colors.length; i++) {
            colors[i] = 0xffffff00;
        }

        return _bitmap.createBitmap(colors, width, height, Bitmap.Config.ARGB_8888);
//        createBitmap(@NonNull @ColorInt int[] colors,int width, int height, Config config)
    }

    public void putImageData(Bitmap imgData, int x, int y) {
        _canvas.drawBitmap(imgData, x, y, _paint);
    }

    public void putImageData(Bitmap imgData, int x, int y, int dirtyX, int dirtyY, int dirtyWidth, int dirtyHeight) {
        RectF rectF = new RectF(x + dirtyX, y + dirtyY, x + dirtyX + dirtyWidth, y + dirtyY + dirtyHeight);
        _canvas.drawBitmap(imgData, null, rectF, _paint);
    }

    public Bitmap getImageData(int x, int y, int width, int height) {
        Bitmap copy = _bitmap.copy(Bitmap.Config.ARGB_8888, true);
        Bitmap bitmap = Bitmap.createBitmap(copy, x, y, width, height);
        return bitmap;
    }


    protected int _fillColor = 0;

    public void setFillColor(int color) {
        _fillColor = color;
    }

    public void setTextSize(float textSize) {
        _textSize = textSize;
    }

    public void setTextColor(int textColor) {
        _textColor = textColor;
    }

    public void setStrokeWidth(int width) {
        _strokeWidth = width;
    }

    protected int _strokeColor = 0;

    public void setStrokeColor(int color) {
        _strokeColor = color;
    }

    @Override
    public Bitmap getBitmap() {
        return _bitmap;
    }

    public Paint getPaint() {
        return _paint;
    }

    protected void finalize() throws Throwable {
        _bitmap.recycle();
        super.finalize();
    }

    public android.graphics.drawable.Drawable getDrawable() {
        return new Drawable();
    }

    public android.graphics.drawable.Drawable copyDrawable() {
        return new BitmapDrawable();
    }

    private class BitmapDrawable extends android.graphics.drawable.Drawable {

        private final Paint _paint = new Paint(Paint.ANTI_ALIAS_FLAG);
        private final Bitmap _bitmap;

        public BitmapDrawable() {
            super();
            _bitmap = KerCanvas.this._bitmap.copy(Bitmap.Config.ARGB_8888, false);
            setBounds(0, 0, _bitmap.getWidth(), _bitmap.getHeight());
        }

        protected void finalize() throws Throwable {
            _bitmap.recycle();
            super.finalize();
        }

        @Override
        public void draw(Canvas canvas) {
            canvas.drawBitmap(_bitmap, new android.graphics.Rect(0, 0, _bitmap.getWidth(), _bitmap.getHeight()), getBounds(), _paint);
        }

        @Override
        public void setAlpha(int i) {
            _paint.setAlpha(i);
            invalidateSelf();
        }

        @Override
        public void setColorFilter(ColorFilter colorFilter) {
            _paint.setColorFilter(colorFilter);
            invalidateSelf();
        }

        @Override
        public int getOpacity() {
            return PixelFormat.TRANSLUCENT;
        }

    }

    private class Drawable extends android.graphics.drawable.Drawable {

        private final Paint _paint = new Paint(Paint.ANTI_ALIAS_FLAG);

        public Drawable() {
            super();
            setBounds(0, 0, _bitmap.getWidth(), _bitmap.getHeight());
        }

        @Override
        public void draw(Canvas canvas) {
            canvas.drawBitmap(_bitmap, new android.graphics.Rect(0, 0, _bitmap.getWidth(), _bitmap.getHeight()), getBounds(), _paint);
        }

        @Override
        public void setAlpha(int i) {
            _paint.setAlpha(i);
            invalidateSelf();
        }

        @Override
        public void setColorFilter(ColorFilter colorFilter) {
            _paint.setColorFilter(colorFilter);
            invalidateSelf();
        }

        @Override
        public int getOpacity() {
            return PixelFormat.TRANSLUCENT;
        }

    }


}
