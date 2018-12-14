package cn.kkmofang.ker;

import android.app.Activity;
import android.app.Fragment;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import java.util.Map;

/**
 * Created by zhanghailong on 2018/12/13.
 */

public class PageFragment extends Fragment implements Page.Listener{

    public final static String TYPE_PAGE = "app";
    public final static String TYPE_WINDOW = "window";

    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

    }

    public String type = TYPE_PAGE;
    public String path;
    public Map<String,String> query;

    protected Page _page;

    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        if(TYPE_WINDOW.equals(type)) {
            return new PageView(inflater.getContext());
        }
        return inflater.inflate(R.layout.page,null);
    }

    protected ViewGroup _topbarView;
    protected ViewGroup _leftView;
    protected ViewGroup _titleView;
    protected ViewGroup _rightView;
    protected TextView _textView;

    public void onViewCreated(View view, Bundle savedInstanceState) {
        super.onViewCreated(view,savedInstanceState);

        _topbarView = view.findViewById(R.id.ker_topbarView);
        _leftView = view.findViewById(R.id.ker_leftView);
        _rightView = view.findViewById(R.id.ker_rightView);
        _textView = view.findViewById(R.id.ker_textView);
        _titleView = view.findViewById(R.id.ker_titleView);

    }

    protected void onWillPageLoad(Page page) {

    }

    protected void onDidPageLoad(Page page) {

    }

    protected void onWillPageUnLoad(Page page) {

    }

    public void onActivityCreated(Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);

        Activity a = getActivity();

        if(a instanceof IAppActivity && path != null) {
            _page = new Page(getView(),((IAppActivity) a).app());
            _page.setListener(this);
            onWillPageLoad(_page);
            _page.run(path,query);
            onDidPageLoad(_page);
        }

        this.onDestroy();
    }

    @Override
    public void onOptions(Object options) {

        if(_textView != null) {
            String v = Ker.getString(options,"title",null);
            if(v != null) {
                _textView.setText(v);
            }
        }

        if(_topbarView != null) {

            Object v = Ker.get(options,"topbar");

            if(v != null) {
                _topbarView.setVisibility(Ker.booleanValue(v,true) ? View.VISIBLE:View.GONE);
            }
        }
    }

    @Override
    public void onDestroy() {

        if(_page != null) {
            onWillPageUnLoad(_page);
            _page.recycle();
            _page = null;
        }

        super.onDestroy();
    }

    @Override
    public void onClose(boolean animated) {

        Activity a = getActivity();

        if(a != null && a instanceof IAppActivity) {
            if(TYPE_WINDOW.equals(type)) {
                ((IAppActivity) a).close(this,animated);
            } else {
                ((IAppActivity) a).back(1,animated);
            }
        }
    }
}
