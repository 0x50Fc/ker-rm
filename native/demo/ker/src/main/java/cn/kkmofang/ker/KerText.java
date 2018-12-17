package cn.kkmofang.ker;

import android.graphics.Canvas;
import android.graphics.Paint;
import android.os.Build;
import android.text.Layout;
import android.text.StaticLayout;
import android.text.TextPaint;
import android.view.View;

import java.lang.ref.WeakReference;

/**
 * Created by zhanghailong on 2018/4/18.
 */

public class KerText {

    public float maxWidth = Integer.MAX_VALUE;
    public float lineSpacing = 0;
    public float letterSpacing = 0;
    public int textAlign = View.TEXT_ALIGNMENT_VIEW_START;
    public final TextPaint paint = new TextPaint(Paint.ANTI_ALIAS_FLAG);
    public float paddingLeft = 0;
    public float paddingTop = 0;

    private StaticLayout _layout;
    private float _width;
    private final WeakReference<TextContent> _textContent;

    public KerText(TextContent textContent) {
        _textContent = new WeakReference<>(textContent);
    }

    public void setNeedDisplay() {
        _layout = null;
    }

    public boolean isNeedDisplay() {
        return _layout == null;
    }

    public void setMaxWidth(int v) {
        if(maxWidth != v) {
            maxWidth = v;
            setNeedDisplay();
        }
    }

    public void setLineSpacing(int v) {
        if(lineSpacing != v) {
            lineSpacing = v;
            setNeedDisplay();
        }
    }

    public void setLetterSpacing(int v) {
        if(letterSpacing != v) {
            letterSpacing = v;
            setNeedDisplay();
        }
    }


    public void setTextAlign(int v) {
        if(textAlign != v) {
            textAlign = v;
            setNeedDisplay();
        }
    }

    public void draw(Canvas canvas,int width,int height) {

        build();
        canvas.translate(paddingLeft,paddingTop + (height - _layout.getHeight()) * 0.5f);
        _layout.draw(canvas);
    }

    private void build() {

        if(_layout == null) {

            CharSequence textContent = "";

            TextContent vv = _textContent.get();


            if(vv != null) {
                textContent = vv.textContent();
            }

            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
                if(paint.getTextSize() != 0.0f) {
                    paint.setLetterSpacing((letterSpacing + 1.3f) / paint.getTextSize());
                }
            }

            Layout.Alignment align = Layout.Alignment.ALIGN_NORMAL;

            if(textAlign == View.TEXT_ALIGNMENT_CENTER) {
                align = Layout.Alignment.ALIGN_CENTER;
            } else if(textAlign == View.TEXT_ALIGNMENT_VIEW_END) {
                align = Layout.Alignment.ALIGN_OPPOSITE;
            }

            if(maxWidth == Integer.MAX_VALUE) {
                align =  Layout.Alignment.ALIGN_NORMAL;
            }

            _layout = new StaticLayout(
                   textContent,
                    0,
                   textContent.length(),
                    paint,
                   (int) Math.ceil(maxWidth),
                    align,
                    1.0f,
                    0f,
                    false);

            _width = 0;

            for(int i=0;i<_layout.getLineCount();i++) {
                float v = _layout.getLineWidth(i);
                if(v > _width) {
                    _width = v;
                }
            }

        }

    }

    public float width() {
        build();
        return _width;
    }

    public float height() {
        build();
        return _layout.getHeight();
    }

    public static interface TextContent {
        CharSequence textContent();
    }
}
