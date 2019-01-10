//
// Created by hailong11 on 2018/12/12.
//

#include "KerImage.h"
#include "kk.h"
#include "global.h"

namespace kk {

    namespace ui {

        NativeImage::NativeImage(kk::DispatchQueue * queue,kk::CString src):_src(src),_state(ImageStateNone),_width(0),_height(0),_queue(queue) {


            this->retain();

            jboolean isAttach = false;

            JNIEnv * env = kk_env(&isAttach);

            jstring URI = src == nullptr ? nullptr : env->NewStringUTF(src);

            env->CallStaticVoidMethod(G.UI.isa,G.UI.getImage,(jlong) this,URI);

            if(URI) {
                env->DeleteLocalRef(URI);
            }

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }

        }

        kk::Native * NativeImage::native() {
            if(_native != nullptr) {
                return _native->native();
            }
            return nullptr;
        }

        void NativeImage::setNative(kk::Native * native) {
            setNative(native,0,0);
        }

        void NativeImage::setNative(kk::Native * native,kk::Uint width,kk::Uint height) {
            if(native == nullptr) {
                _native = nullptr;
                _width = 0;
                _height = 0;
            } else {
                _native = new kk::NativeObject(native);
                _width = width;
                _height = height;
            }
        }


        ImageState NativeImage::state() {
            return _state;
        }

        kk::Uint NativeImage::width() {
            return _width;
        }

        kk::Uint NativeImage::height() {
            return _height;
        }

        kk::CString NativeImage::src() {
            return _src.c_str();
        }

        void NativeImage::copyPixels(void * data) {

            if(_native != nullptr && data != nullptr) {

                jboolean isAttach = false;

                JNIEnv * env = kk_env(&isAttach);

                jbyteArray b = (jbyteArray) env->CallStaticObjectMethod(G.UI.isa,G.UI.copyPixels,(jobject) _native->native());

                if(b != nullptr) {

                    size_t n = _width * _height * 4;

                    if(env->GetArrayLength(b) == n) {
                        jbyte * v = env->GetByteArrayElements(b, nullptr);
                        memcpy(data,v, n);
                        env->ReleaseByteArrayElements(b,v,JNI_COMMIT);
                    }

                    env->DeleteLocalRef(b);
                }

                if(isAttach) {
                    gJavaVm->DetachCurrentThread();
                }

            }

        }

        Boolean NativeImage::isCopyPixels() {
            return _width > 0 && _height > 0 && _native != nullptr ;
        }

        void NativeImage::setState(ImageState state) {
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

        kk::DispatchQueue * NativeImage::queue() {
            return _queue;
        }

        jobject GetImageObject(Image * image) {
            if(image == nullptr) {
                return nullptr;
            }
            {
                NativeImage * v = dynamic_cast<NativeImage *>(image);
                if(v != nullptr) {
                    return (jobject) v->native();
                }
            }
            return nullptr;
        }
    }
}
