package cn.kkmofang.ker;

import android.app.Fragment;

/**
 * Created by hailong11 on 2018/12/13.
 */

public interface IAppActivity {
    void open(Fragment fragment,boolean animated);
    void back(int delta,boolean animated);
    void show(Fragment fragment,boolean animated);
    void close(Fragment fragment,boolean animated);
}
