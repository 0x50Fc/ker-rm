//
// Created by zhanghailong on 2018/12/13.
//

#include "KerApp.h"
#include "kk.h"

namespace kk {

    namespace ui {

        KerApp::KerApp(jweak object,kk::CString basePath,kk::CString userAgent,kk::CString appkey):App(basePath,"Android",userAgent,appkey),_object(object){

        }

        KerApp::~KerApp() {

            jboolean isAttach = false;

            JNIEnv *env = kk_env(&isAttach);

            env->DeleteWeakGlobalRef(_object);

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }
        }

        jweak KerApp::object() {
            return _object;
        }

    }
}
