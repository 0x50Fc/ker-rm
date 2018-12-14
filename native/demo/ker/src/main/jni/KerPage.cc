//
// Created by hailong11 on 2018/12/14.
//

#include "KerPage.h"
#include "kk.h"

namespace kk {

    namespace ui {

        KerPage::KerPage(App * app,View * view,jweak object):Page(app,view),_object(object) {

        }

        KerPage::~KerPage() {

            if(_object) {

                jboolean isAttach = false;

                JNIEnv *env = kk_env(&isAttach);

                env->DeleteWeakGlobalRef(_object);

                if(isAttach) {
                    gJavaVm->DetachCurrentThread();
                }

            }
        }

        jweak KerPage::object(){
            return _object;
        }

    }

}
