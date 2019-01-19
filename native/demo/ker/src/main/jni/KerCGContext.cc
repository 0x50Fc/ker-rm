//
// Created by zhanghailong on 2018/12/18.
//

#include "KerCGContext.h"
#include "kk.h"
#include <ui/view.h>
#include "global.h"
#include <android/bitmap.h>

namespace kk {

    namespace ui {

        void displayCGContext(kk::ui::CG::Context * context,jlong viewId,jlong appid) {

            if(context == nullptr) {
                return;
            }

            jboolean isAttach = false;

            JNIEnv *env = kk_env(&isAttach);

            kk::PointerObject * v = (kk::PointerObject *) context->object();

            jobject bitmap;

            if(v == nullptr) {
                jobject vv = env->CallStaticObjectMethod(G.UI.isa,G.UI.createBitmap,(jint) context->width(),(jint) context->height());
                bitmap = env->NewGlobalRef(vv);
                env->DeleteLocalRef(vv);
                v = new kk::PointerObject(bitmap,[](void * pointer)->void{

                    jobject bitmap = (jobject) pointer;

                    jboolean isAttach = false;

                    JNIEnv *env = kk_env(&isAttach);

                    env->CallVoidMethod(bitmap,G.Bitmap.recycle);

                    env->DeleteGlobalRef(bitmap);

                    if(isAttach) {
                        gJavaVm->DetachCurrentThread();
                    }

                });
                context->setObject(v);
            } else {
                bitmap = (jobject) v->pointer();
            }

            void * dest = nullptr;

            if( 0 == AndroidBitmap_lockPixels(env,bitmap,& dest)) {
                context->copyPixels(dest);
                AndroidBitmap_unlockPixels(env,bitmap);
            }

            env->CallStaticVoidMethod(G.UI.isa,G.UI.displayCanvas, bitmap,viewId,appid);

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }

        }

    }

}

