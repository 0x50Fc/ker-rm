package cn.kkmofang.ker;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.drawable.Drawable;
import android.view.View;
import android.view.ViewGroup;

/**
 * Created by zhanghailong on 2018/4/19.
 */

public class KerTextView extends View implements IKerView,KerText.TextContent {

    private final KerText _text = new KerText(this);
    private CharSequence _textContent;


    public KerTextView(Context context) {
        super(context);
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        _text.setMaxWidth(getWidth());
        _text.draw(canvas,getWidth(),getHeight());
    }

    @Override
    public void setViewConfiguration(long viewConfiguration) {

    }

    @Override
    public void setAttributeValue(long object, String key, String value) {

        if("color".equals(key)) {
            _text.paint.setColor(Ker.colorValue(value,0xff000000));
        } else if("font".equals(key)) {
            Font font = Ker.fontValue(value,new Font());
            _text.paint.setTextSize(font.size);
            _text.paint.setFakeBoldText(font.weight == Font.WEIGHT_BOLD);
        } else if("text-align".equals(key)) {
            if("center".equals(value)) {
                _text.textAlign = View.TEXT_ALIGNMENT_CENTER;
            } else if("right".equals(value)) {
                _text.textAlign = View.TEXT_ALIGNMENT_VIEW_END;
            } else {
                _text.textAlign = View.TEXT_ALIGNMENT_VIEW_START;
            }
        } else if("#text".equals(key)) {
            setTextContent(value);
        }

        KerView.setAttributeValue(this,object,key,value);

        _text.setNeedDisplay();
        invalidate();
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

    @Override
    public CharSequence textContent() {
        return _textContent;
    }

    public void setTextContent(CharSequence v) {
        _textContent = v;
        _text.setNeedDisplay();
        invalidate();
    }
}
