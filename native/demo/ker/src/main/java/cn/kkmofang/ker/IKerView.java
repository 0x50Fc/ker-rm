package cn.kkmofang.ker;

import android.graphics.drawable.Drawable;
import android.view.ViewGroup;

/**
 * Created by hailong11 on 2018/12/17.
 */

public interface IKerView {
    void setViewConfiguration(long viewConfiguration);
    void setAttributeValue(long object,String key,String value);
    void recycle(long object);
    void obtain(long object);
    void setContent(long object,String content,String contentType,String basePath);
    void evaluateJavaScript(long object,String evaluateCode);
    void setImage(Drawable image);
    ViewGroup contentView();
}
