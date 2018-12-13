package cn.kkmofang.ker;

import android.app.Activity;
import android.app.Fragment;
import android.os.Bundle;

/**
 * Created by hailong11 on 2018/12/13.
 */

public class PageFragment extends Fragment implements Page.Listener{

    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

    }

    protected Page _page;

    public void onActivityCreated(Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);

        Activity a = getActivity();

        if(a instanceof IAppActivity) {
            _page = new Page(getView(),((IAppActivity) a).app());
            _page.setListener(this);
        }
    }

    @Override
    public void onOptions(Object options) {

    }

    @Override
    public void onClose(boolean animated) {

    }
}
