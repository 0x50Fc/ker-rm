package cn.kkmofang.ker;

import android.content.Context;
import android.graphics.PixelFormat;
import android.view.SurfaceView;

/**
 * Created by hailong11 on 2019/1/17.
 */

public class KerSurfaceView extends SurfaceView {

    public KerSurfaceView(Context context) {
        super(context);

        setZOrderOnTop(true);
        getHolder().setFormat(PixelFormat.TRANSLUCENT);
        setBackgroundColor(0x00000000);

    }

}
