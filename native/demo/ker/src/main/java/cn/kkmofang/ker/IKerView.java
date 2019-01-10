package cn.kkmofang.ker;

import android.graphics.drawable.Drawable;
import android.view.ViewGroup;

/**
 * Created by hailong11 on 2018/12/17.
 */

public interface IKerView {
    void setViewConfiguration(long viewId,WebViewConfiguration viewConfiguration,long appid);
    void setAttributeValue(long viewId,String key,String value,long appid);
    void recycle(long viewId,long appid);
    void obtain(long viewId,long appid);
    void setContent(long viewId,String content,String contentType,String basePath,long appid);
    void evaluateJavaScript(long viewId,String evaluateCode,long appid);
    void setImage(long viewId,Object image,long appid);
    void setAttributedText(long viewId,CharSequence string,long appid);
    void setContentSize(long viewId,int width,int height,long appid);
    void setContentOffset(long viewId,int x,int y,boolean animated,long appid);
    ViewGroup contentView();
}
