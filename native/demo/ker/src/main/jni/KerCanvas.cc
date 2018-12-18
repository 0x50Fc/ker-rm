//
// Created by zhanghailong on 2018/12/12.
//

#include "KerCanvas.h"
#include "kk.h"
#include "KerCGContext.h"

namespace kk {

    namespace ui {

        OSCanvas::OSCanvas(DispatchQueue * queue):OSCanvas(queue, nullptr) {

        }

        OSCanvas::OSCanvas(DispatchQueue * queue, jobject object):Canvas(queue),_width(0),_height(0),_resize(false),_object(nullptr) {

            if(object) {

                jboolean isAttach = false;

                JNIEnv *env = kk_env(&isAttach);

                jclass isa = env->FindClass("cn/kkmofang/ker/Native");

                _object = env->NewGlobalRef(object);

                jmethodID getViewWidth = env->GetStaticMethodID(isa,"getViewWidth","(Ljava/lang/Object;)I");
                jmethodID getViewHeight = env->GetStaticMethodID(isa,"getViewHeight","(Ljava/lang/Object;)I");

                jint width = env->CallStaticIntMethod(isa,getViewWidth,object);
                jint height = env->CallStaticIntMethod(isa,getViewHeight,object);

                setWidth((kk::Uint) width);
                setHeight((kk::Uint) height);

                env->DeleteLocalRef(isa);

                if(isAttach) {
                    gJavaVm->DetachCurrentThread();
                }

            }
        }

        OSCanvas::~OSCanvas() {
            if(_object != nullptr) {

                jboolean isAttach = false;

                JNIEnv *env = kk_env(&isAttach);

                env->DeleteGlobalRef(_object);

                if(isAttach) {
                    gJavaVm->DetachCurrentThread();
                }

            }
        }

        Strong<Object> OSCanvas::getContext(kk::CString name) {

            if(name == nullptr ){
                return nullptr;
            }

            if(_width == 0 || _height == 0) {
                return nullptr;
            }

            if(strcmp(name, "2d") == 0) {

                kk::Strong<kk::ui::CG::Context> v = (kk::ui::CG::Context *) _context;

                if(v == nullptr || _resize) {
                    v = createCGContext(_width,_height);
                    _context = v;
                } else {
                    v->clearRect(0, 0, _width, _height);
                }

                if(_object != nullptr) {
                    kk::Weak<OSCanvas> canvas = this;
                    _queue->async([v,canvas]()->void{
                        kk::Strong<OSCanvas> c = canvas.operator->();
                        if(c != nullptr) {
                            displayCGContext(v.operator->(),c->_object);
                        }
                    });
                }

            }

            _resize = false;

            return _context;
        }

        Uint OSCanvas::width() {
            return _width;
        }

        void OSCanvas::setWidth(Uint v) {
            if(_width != v) {
                _width = v;
                _resize = true;
            }
        }

        Uint OSCanvas::height() {
            return _height;
        }

        void OSCanvas::setHeight(Uint v) {
            if(_height != v) {
                _height = v;
                _resize = true;
            }
        }

        Strong<Image> OSCanvas::toImage() {
            return nullptr;
        }

        kk::Strong<Canvas> createCanvas(DispatchQueue * queue) {
            return new OSCanvas(queue);
        }

    }
}
