package cn.kkmofang.ker;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.ColorFilter;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.PixelFormat;
import android.graphics.RectF;

/**
 * Created by zhanghailong on 2018/12/18.
 */

public class KerCanvas extends Canvas implements Image {

    protected Bitmap _bitmap;
    protected final Paint _paint = new Paint(Paint.ANTI_ALIAS_FLAG);
    protected final Path _path = new Path();

    public KerCanvas(int width,int height) {
        super();
        _bitmap = Bitmap.createBitmap(width,height, Bitmap.Config.ARGB_8888);
        setBitmap(_bitmap);
    }

    public void clearRect(int x,int y,int width,int height) {
        int r = Math.min(_bitmap.getWidth(),x + width);
        int b = Math.min(_bitmap.getHeight(),y + height);
        for(int l = x; l < r;l ++) {
            for(int t = y; t < b;t ++) {
                _bitmap.setPixel(l,t,0);
            }
        }
    }

    public void rect(float x,float y,float width,float height) {
        _path.addRect(x,y,x + width,y + height, Path.Direction.CW);
    }

    public void fill() {
        _paint.setStyle(Paint.Style.FILL);
        drawPath(_path,_paint);
        _path.reset();
    }

    public void stroke() {
        _paint.setStyle(Paint.Style.STROKE);
        drawPath(_path,_paint);
        _path.reset();
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
        clipPath(_path);
    }

    public void quadraticCurveTo(float cpx,float cpy,float x,float y ) {
        _path.quadTo(cpx,cpy,x,y);
    }

    public void bezierCurveTo(float cp1x,float cp1y,float cp2x,float cp2y,float x,float y) {
        _path.cubicTo(cp1x,cp1y,cp2x,cp2y,x,y);
    }

    public void arc(float x,float y,float r, float sAngle,float eAngle,boolean counterclockwise) {
        if(!counterclockwise) {
            float v = sAngle;
            sAngle = eAngle;
            eAngle = v;
        }
        _path.arcTo(new RectF(x - r,y - r,x  +r,y + r),sAngle,eAngle,false);
    }

    public void arcTo(float x1,float y1,float x2,float y2,float r) {
        //TODO
    }

    public boolean isPointInPath(float x,float y) {
        //TODO
        return false;
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

    private class Drawable extends android.graphics.drawable.Drawable {

        private Paint _paint = new Paint(Paint.ANTI_ALIAS_FLAG);

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
