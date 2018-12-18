package cn.kkmofang.ker;

import android.content.Context;
import android.graphics.drawable.Drawable;
import android.util.AttributeSet;
import android.view.View;
import android.view.ViewGroup;

/**
 * Created by zhanghailong on 2018/12/14.
 */

public class KerView extends ViewGroup implements IKerView {

    public KerView(Context context) {
        super(context);
    }

    public KerView(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {

        int width = getDefaultSize(Integer.MAX_VALUE,widthMeasureSpec);
        int height = getDefaultSize(Integer.MAX_VALUE,heightMeasureSpec);

        Rect frame = (Rect) getTag(R.id.ker_frame);

        if(frame != null) {
            width = frame.width;
            height = frame.height;
        }

        setMeasuredDimension(width, height);

        int parentWidthSpec = MeasureSpec.makeMeasureSpec(width, MeasureSpec.AT_MOST);
        int parentHeightSpec = MeasureSpec.makeMeasureSpec(height, MeasureSpec.AT_MOST);

        int childCount = getChildCount();
        for (int i = 0; i < childCount; i++) {
            View v = getChildAt(i);
            if (v != null && v.getVisibility() != GONE){
                measureChild(v, parentWidthSpec, parentHeightSpec);
            }
        }

    }

    @Override
    protected void onLayout(boolean changed, int l, int t, int r, int b) {
        int count = getChildCount();

        for(int i=0;i<count;i++) {

            View v = getChildAt(i);

            if(v != null && v.getVisibility() == View.VISIBLE) {

                Rect frame = (Rect) v.getTag(R.id.ker_frame);

                if(frame != null) {
                    v.layout(frame.x,frame.y,frame.x + frame.width,frame.y + frame.height);
                }

            }

        }
    }

    @Override
    public void setViewConfiguration(long viewConfiguration) {

    }

    @Override
    public void setAttributeValue(long object, String key, String value) {
        setAttributeValue(this,object,key,value);
    }

    public static void setAttributeValue(View view,long object, String key, String value) {

        if("background-color".equals(key)) {
            view.setBackgroundColor(Ker.colorValue(value,0));
        } else if("hidden".equals(key)) {
            if(Ker.booleanValue(value,false)) {
                view.setVisibility(View.GONE);
            } else {
                view.setVisibility(View.VISIBLE);
            }
        }

    }

    @Override
    public void recycle(long object) {

    }

    @Override
    public void obtain(long object) {

    }

    @Override
    public void setContent(long object, String content, String contentType, String basePath) {

    }

    @Override
    public void evaluateJavaScript(long object, String evaluateCode) {

    }

    @Override
    public void setImage(Drawable image) {
        setBackground(image);
    }

    @Override
    public ViewGroup contentView() {
        return null;
    }
}
