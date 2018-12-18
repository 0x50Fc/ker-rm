//
// Created by hailong11 on 2018/12/12.
//

#include "KerImage.h"
#include "kk.h"
#include "KerApp.h"

namespace kk {

    namespace ui {

        OSImage::OSImage(kk::CString src):_state(ImageStateNone),_src(src),_object(nullptr),_width(0),_height(0) {

        }

        OSImage::OSImage(jobject object):_state(ImageStateLoaded),_object(nullptr) {
            setImage(object);
        }

        OSImage::~OSImage() {

            if(_object) {

                jboolean isAttach = false;

                JNIEnv *env = kk_env(&isAttach);

                env->DeleteGlobalRef(_object);

                if(isAttach) {
                    gJavaVm->DetachCurrentThread();
                }

            }

        }

        ImageState OSImage::state() {
            return _state;
        }

        kk::Uint OSImage::width() {
            return _width;
        }

        kk::Uint OSImage::height() {
            return _height;
        }

        kk::CString OSImage::src() {
            return _src.c_str();
        }

        void OSImage::setSrc(kk::CString src) {
            _src = src;
        }

        void OSImage::copyPixels(void * data) {

            jboolean isAttach = false;

            JNIEnv * env = kk_env(&isAttach);

            if(_object && _width > 0 && _height > 0) {



            }

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }

        }

        Boolean OSImage::isCopyPixels() {
            return _object != nullptr;
        }

        void OSImage::setImage(jobject object) {

            if(_object != object) {

                jboolean isAttach = false;

                JNIEnv *env = kk_env(&isAttach);

                jobject v = _object;

                if(object) {
                    _object = env->NewGlobalRef(object);
                } else {
                    _object = nullptr;
                }

                if(v) {
                    env->DeleteGlobalRef(v);
                }

                if(_object) {

                    jclass isa = env->FindClass("cn/kkmofang/ker/Native");

                    jmethodID getImageWidth = env->GetStaticMethodID(isa,"getImageWidth","(Ljava/lang/Object;)I");

                    _width = (kk::Uint) env->CallStaticIntMethod(isa,getImageWidth,_object);

                    jmethodID getImageHeight = env->GetStaticMethodID(isa,"getImageHeight","(Ljava/lang/Object;)I");

                    _height = (kk::Uint) env->CallStaticIntMethod(isa,getImageHeight,_object);

                } else {
                    _width = 0;
                    _height = 0;
                }

                if(isAttach) {
                    gJavaVm->DetachCurrentThread();
                }

            }


        }

        void OSImage::setState(ImageState state) {
            if(_state != state) {
                _state = state;
                if(state == ImageStateError) {
                    Strong<Event> e = new Event();
                    emit("error", e);
                } else if(state == ImageStateLoaded) {
                    Strong<Event> e = new Event();
                    emit("load", e);
                }
            }
        }

        jobject OSImage::object() {
            return _object;
        }

        kk::Strong<Image> ImageCreate(Context * context,kk::CString src) {

            kk::ui::KerApp * app = nullptr;
            kk::Strong<Context> v = context;

            while(v != nullptr && app == nullptr) {
                app = dynamic_cast<kk::ui::KerApp *>((Context *) v);
                if(app != nullptr) {
                    break;
                }
                v = v->parent();
            }

            if(app == nullptr) {
                return nullptr;
            }


            OSImage * i = new OSImage(src);

            jboolean isAttach = false;

            JNIEnv * env = kk_env(&isAttach);

            jclass isa = env->FindClass("cn/kkmofang/ker/Native");

            jmethodID getImage = env->GetStaticMethodID(isa,"getImage","(Lcn/kkmofang/ker/App;JLjava/lang/String;Ljava/lang/String;J)V");

            jstring URI = src == nullptr ? nullptr : env->NewStringUTF(src);
            jstring basePath = env->NewStringUTF(context->basePath());

            env->CallStaticVoidMethod(isa,getImage,app->object(),(jlong) i,URI,basePath,(jlong) context->queue());

            if(URI) {
                env->DeleteLocalRef(URI);
            }

            if(basePath) {
                env->DeleteLocalRef(basePath);
            }

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }

            return i;

        }
    }
}
