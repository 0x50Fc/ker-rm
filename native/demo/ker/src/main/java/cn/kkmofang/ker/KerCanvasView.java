package cn.kkmofang.ker;

import android.content.Context;
import android.graphics.*;
import android.graphics.Rect;
import android.graphics.drawable.Drawable;
import android.os.Handler;
import android.os.HandlerThread;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import java.util.LinkedList;
import java.util.Queue;

/**
 * Created by zhanghailong on 2018/12/24.
 */

public class KerCanvasView extends SurfaceView implements SurfaceHolder.Callback {

    public KerCanvasView(Context context) {
        super(context);

        setZOrderOnTop(true);
        getHolder().addCallback(this);
        getHolder().setFormat(PixelFormat.TRANSLUCENT);
        setBackgroundColor(0x00000000);

    }

    protected boolean _isCreated = false;

    protected Queue<Drawable> _drawables = new LinkedList<>();

    public void post(final Drawable drawable) {
        drawable.setBounds(new Rect(0,0,getWidth(),getHeight()));
        SurfaceHolder holder = getHolder();
        if(_isCreated) {
            Canvas canvas = holder.lockCanvas();
            canvas.drawColor(Color.TRANSPARENT, PorterDuff.Mode.CLEAR);
            drawable.draw(canvas);
            holder.unlockCanvasAndPost(canvas);
        } else {
            _drawables.offer(drawable);
        }
    }


    @Override
    public void surfaceCreated(final SurfaceHolder holder) {
        _isCreated = true;
        Canvas canvas = holder.lockCanvas();
        canvas.drawColor(Color.TRANSPARENT, PorterDuff.Mode.CLEAR);
        while(!_drawables.isEmpty()) {
            Drawable drawable = _drawables.poll();
            drawable.draw(canvas);
        }
        holder.unlockCanvasAndPost(canvas);
    }

    @Override
    public void surfaceChanged(SurfaceHolder surfaceHolder, int i, int i1, int i2) {

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder surfaceHolder) {
        _isCreated = false;
    }
}
