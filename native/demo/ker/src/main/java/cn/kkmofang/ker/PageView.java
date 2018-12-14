package cn.kkmofang.ker;

import android.content.Context;
import android.util.AttributeSet;
import android.view.View;
import android.view.ViewGroup;

/**
 * Created by zhanghailong on 2018/12/14.
 */

public class PageView extends ViewGroup {

    public PageView(Context context) {
        super(context);
    }

    public PageView(Context context, AttributeSet attrs) {
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

                Rect frame = (Rect) getTag(R.id.ker_frame);

                if(frame != null) {
                    v.layout(frame.x,frame.y,frame.x + frame.width,frame.y + frame.height);
                }

            }

        }
    }

}
