//
// Created by hailong11 on 2018/12/12.
//

#include "KerPackage.h"
#include <core/crypto.h>
#include "kk.h"
#include "KerApp.h"
#include "KerObject.h"
#include "global.h"
namespace kk {

    namespace ui {


        OSPackage::OSPackage(Context * context,kk::CString URI):Package(context,URI) {

        }

        void OSPackage::run(kk::Object * query) {

            kk::ui::KerApp * app = _context;

            if(app == nullptr) {
                return ;
            }

            jboolean isAttach = false;

            JNIEnv *env = kk_env(&isAttach);

            jstring u = env->NewStringUTF(_URI.c_str());

            jobject q = nullptr;

            kk::JSObject * v = dynamic_cast<kk::JSObject *>(query);

            if(v) {
                duk_context * ctx = v->jsContext();
                void * heapptr = v->heapptr();
                if(ctx && heapptr) {
                    duk_push_heapptr(ctx,heapptr);
                    q = duk_to_JObject(env,ctx,-1);
                    duk_pop(ctx);
                }
            }

            env->CallStaticVoidMethod(G.Native.isa,G.Native.runApp,app->object(), u,q);

            env->DeleteLocalRef(u);

            if(q) {
                env->DeleteLocalRef(q);
            }

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }

        }

        kk::Strong<Package> createPackage(Context * context,kk::CString URI) {

            if(URI == nullptr) {
                return nullptr;

            }

            kk::ui::KerApp * app = dynamic_cast<kk::ui::KerApp *>(context);

            if(app == nullptr) {
                return nullptr;
            }

            kk::Strong<Package> v = new OSPackage(context,URI);

            jboolean isAttach = false;

            JNIEnv *env = kk_env(&isAttach);

            jstring u = env->NewStringUTF(URI);

            env->CallStaticVoidMethod(G.Native.isa,G.Native.getPackage,app->object(), (jlong) v.get(), u);

            env->DeleteLocalRef(u);

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }

            return v;
        }

    }

}