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
        if(savedInstanceState != null) {
            _type = savedInstanceState.getString("type",TYPE_PAGE);
            _pageId = savedInstanceState.getLong("pageId",0);
        } else {
            Bundle b = getArguments();
            if(b != null) {
                _type = b.getString("type",TYPE_PAGE);
                _pageId = b.getLong("pageId",0);
            }
        }
    }

    protected String _type;
    protected long _pageId;

    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        if(TYPE_WINDOW.equals(_type)) {
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

    protected void onWillPageAppear(Page page) {
        page.emit("willAppear",null);
    }

    protected void onDidPageAppear(Page page) {
        page.emit("didAppear",null);
    }

    protected void onWillPageDisappear(Page page) {
        page.emit("willDisappear",null);
    }

    protected void onDidPageDisappear(Page page) {
        page.emit("didDisappear",null);
    }

    protected void onWillPageLoad(Page page) {

    }

    protected void onDidPageLoad(Page page) {

    }

    protected void onWillPageUnLoad(Page page) {

    }

    public void onActivityCreated(Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);

        if(_pageId != 0) {

            View view = getView();

            PageView pageView;

            if(view instanceof PageView) {
                pageView = (PageView) view;
            } else {
                pageView = view.findViewById(R.id.ker_contentView);
            }

            if(pageView != null) {
                Page page = KerUI.getPage(_pageId);
                if (page != null) {
                    page.setListener(this);
                    onWillPageLoad(page);
                    page.open(pageView);
                    onDidPageLoad(page);
                }
            }
        }

    }

    @Override
    public void onStart() {
        super.onStart();

        if(_pageId != 0) {
            Page page = KerUI.getPage(_pageId);
            if (page != null) {
                onWillPageAppear(page);
                onDidPageAppear(page);
            }
        }
    }

    @Override
    public void onStop() {

        if(_pageId != 0) {

            Page page = KerUI.getPage(_pageId);

            if (page != null) {
                onWillPageDisappear(page);
                onDidPageDisappear(page);
            }

        }

        super.onStop();
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

        if(_pageId != 0) {

            Page page = KerUI.getPage(_pageId);

            if(page != null) {
                onWillPageUnLoad(page);
                KerUI.removePage(_pageId);
            }

            _pageId = 0;
        }

        super.onDestroy();
    }

    @Override
    public void onClose(boolean animated) {

        Activity a = getActivity();

        if(a != null && a instanceof IAppActivity) {
            if(TYPE_WINDOW.equals(_type)) {
                ((IAppActivity) a).close(this,animated);
            } else {
                ((IAppActivity) a).back(1,animated);
            }
        }
    }

    @Override
    public void onOpened() {

    }

    public boolean onBackPressed() {

        if(_pageId != 0) {

            Page page = KerUI.getPage(_pageId);

            if(page != null) {

                if(page.isBackPressed()) {

                    page.emit("backPressed",null);

                    return false;
                }

            }
        }

        return true;
    }
}
