package cn.kkmofang.ker;

import android.graphics.*;
import android.graphics.Rect;

/**
 * Created by zhanghailong on 2018/12/18.
 */

public class KerCanvas extends Object implements Image {

    protected final Canvas _canvas;
    protected final Bitmap _bitmap ;
    protected final Paint _paint = new Paint(Paint.ANTI_ALIAS_FLAG);
    protected final Path _path = new Path();

    public KerCanvas(int width,int height) {
        super();
        _bitmap = Bitmap.createBitmap(width,height, Bitmap.Config.ARGB_8888);
        _canvas = new Canvas(_bitmap);
        _canvas.drawColor(Color.TRANSPARENT, PorterDuff.Mode.CLEAR);
    }

    public int getWidth() {
        return _canvas.getWidth();
    }

    public int getHeight() {
        return _canvas.getHeight();
    }

    public void clearRect(int x,int y,int width,int height) {
        Paint p = new Paint(Paint.ANTI_ALIAS_FLAG);
        p.setXfermode(new PorterDuffXfermode(PorterDuff.Mode.CLEAR));
        _canvas.drawRect(new Rect(x,y,x + width,y + height),p);
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

    public void rect(float x,float y,float width,float height) {
        _path.addRect(x,y,x + width,y + height, Path.Direction.CW);
    }

    public void fill() {
        _paint.setColor(_fillColor);
        _paint.setAlpha(0x0ff & (_fillColor >> 16));
        _paint.setStyle(Paint.Style.FILL);
        _canvas.drawPath(_path,_paint);
    }

    public void stroke() {
        _paint.setColor(_strokeColor);
        _paint.setAlpha(0x0ff & (_strokeColor >> 16));
        _paint.setStyle(Paint.Style.STROKE);
        _canvas.drawPath(_path,_paint);
    }

    public void beginPath() {
        _path.reset();
    }

    public void closePath() {
        _path.close();
    }

    public void moveTo(float x,float y) {
        _path.moveTo(x,y);
    }

    public void lineTo(float x,float y) {
        _path.lineTo(x,y);
    }

    public void clip() {
        _canvas.clipPath(_path);
    }

    public void quadraticCurveTo(float cpx,float cpy,float x,float y ) {
        _path.quadTo(cpx,cpy,x,y);
    }

    public void bezierCurveTo(float cp1x,float cp1y,float cp2x,float cp2y,float x,float y) {
        _path.cubicTo(cp1x,cp1y,cp2x,cp2y,x,y);
    }

    public void arc(float x,float y,float r, float sAngle,float eAngle,boolean counterclockwise) {
        sAngle = sAngle * (float) (180.0 / Math.PI);
        eAngle = eAngle * (float) (180.0 / Math.PI);
        _path.addRect(new RectF(x - r,y - r,x  +r,y + r), Path.Direction.CW);
        _path.arcTo(new RectF(x - r,y - r,x  +r,y + r),sAngle,eAngle,true);
    }

    public void arcTo(float x1,float y1,float x2,float y2,float r) {
        //TODO
    }

    public boolean isPointInPath(float x,float y) {
        //TODO
        return false;
    }

    public void scale(float sx,float sy) {
        _canvas.scale(sx,sy);
    }

    protected int _fillColor = 0;

    public void setFillColor(int color) {
        _fillColor = color;
    }

    protected int _strokeColor = 0;

    public void setStrokeColor(int color) {
        _strokeColor = color;
    }

    @Override
    public Bitmap getBitmap() {
        return _bitmap;
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
            _bitmap = KerCanvas.this._bitmap.copy(Bitmap.Config.ARGB_8888,false);
            setBounds(0,0,_bitmap.getWidth(),_bitmap.getHeight());
        }

        protected void finalize() throws Throwable {
            _bitmap.recycle();
            super.finalize();
        }

        @Override
        public void draw(Canvas canvas) {
            canvas.drawBitmap(_bitmap,new android.graphics.Rect(0,0,_bitmap.getWidth(),_bitmap.getHeight()),getBounds(),_paint);
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
            setBounds(0,0,_bitmap.getWidth(),_bitmap.getHeight());
        }

        @Override
        public void draw(Canvas canvas) {
            canvas.drawBitmap(_bitmap,new android.graphics.Rect(0,0,_bitmap.getWidth(),_bitmap.getHeight()),getBounds(),_paint);
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
