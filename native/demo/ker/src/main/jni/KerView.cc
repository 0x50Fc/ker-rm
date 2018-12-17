//
// Created by hailong11 on 2018/12/12.
//

#include "KerView.h"
#include "kk.h"
#include "KerCanvas.h"
#include "KerImage.h"
#include "KerApp.h"

namespace kk {

    namespace ui {

        OSView::OSView(jobject view,ViewConfiguration * configuration,Context * context):View(configuration,context) {

            jboolean isAttach = false;

            JNIEnv *env = kk_env(&isAttach);

            _view = env->NewGlobalRef(view);

            jclass isa = env->FindClass("cn/kkmofang/ker/Native");

            jmethodID viewObtain = env->GetStaticMethodID(isa,"viewObtain","(Ljava/lang/Object;J)V");

            env->CallStaticVoidMethod(isa,viewObtain,_view,(jlong) this);

            env->DeleteLocalRef(isa);

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }

        }

        OSView::~OSView() {

            jboolean isAttach = false;

            JNIEnv *env = kk_env(&isAttach);

            jclass isa = env->FindClass("cn/kkmofang/ker/Native");

            jmethodID viewRecycle = env->GetStaticMethodID(isa,"viewRecycle","(Ljava/lang/Object;J)V");

            env->CallStaticVoidMethod(isa,viewRecycle,_view,(jlong) this);

            env->DeleteLocalRef(isa);

            env->DeleteGlobalRef(_view);

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }

            kk::Log("[OSView] [dealloc]");
        }

        void OSView::set(kk::CString name,kk::CString value) {

            jboolean isAttach = false;

            JNIEnv *env = kk_env(&isAttach);

            jclass isa = env->FindClass("cn/kkmofang/ker/Native");

            jmethodID viewSetAttribute = env->GetStaticMethodID(isa,"viewSetAttribute","(Ljava/lang/Object;JLjava/lang/String;Ljava/lang/String;)V");

            jstring name_ = env->NewStringUTF(name);
            jstring value_ = value == nullptr ? nullptr : env->NewStringUTF(value);

            env->CallStaticVoidMethod(isa,viewSetAttribute,_view,(jlong) this,name_,value_);

            env->DeleteLocalRef(name_);

            if(value_ != nullptr) {
                env->DeleteLocalRef(value_);
            }

            env->DeleteLocalRef(isa);

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }

        }

        void OSView::setFrame(Rect & frame) {

            jboolean isAttach = false;

            JNIEnv *env = kk_env(&isAttach);

            jclass isa = env->FindClass("cn/kkmofang/ker/Native");

            jmethodID viewSetFrame = env->GetStaticMethodID(isa,"viewSetFrame","(Ljava/lang/Object;JFFFF)V");

            env->CallStaticVoidMethod(isa,viewSetFrame,_view,(jlong) this
                    ,(jfloat) frame.origin.x
                    ,(jfloat) frame.origin.y
                    ,(jfloat) frame.size.width
                    ,(jfloat) frame.size.height);

            env->DeleteLocalRef(isa);

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }

        }

        void OSView::setContentSize(Size & size) {

            jboolean isAttach = false;

            JNIEnv *env = kk_env(&isAttach);

            jclass isa = env->FindClass("cn/kkmofang/ker/Native");

            jmethodID viewSetContentSize = env->GetStaticMethodID(isa,"viewSetContentSize","(Ljava/lang/Object;JFF)V");

            env->CallStaticVoidMethod(isa,viewSetContentSize,_view,(jlong) this
                    ,(jfloat) size.width
                    ,(jfloat) size.height);

            env->DeleteLocalRef(isa);

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }

        }

        void OSView::setContentOffset(Point & offset,kk::Boolean animated) {

            jboolean isAttach = false;

            JNIEnv *env = kk_env(&isAttach);

            jclass isa = env->FindClass("cn/kkmofang/ker/Native");

            jmethodID viewSetContentOffset = env->GetStaticMethodID(isa,"viewSetContentOffset","(Ljava/lang/Object;JFFZ)V");

            env->CallStaticVoidMethod(isa,viewSetContentOffset,_view,(jlong) this
                    ,(jfloat) offset.x
                    ,(jfloat) offset.y
                    ,(jboolean) animated);

            env->DeleteLocalRef(isa);

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }

        }

        Point OSView::contentOffset() {
            Point p;

            jboolean isAttach = false;

            JNIEnv *env = kk_env(&isAttach);

            jclass isa = env->FindClass("cn/kkmofang/ker/Native");

            jmethodID viewGetContentOffsetX = env->GetStaticMethodID(isa,"viewGetContentOffsetX","(Ljava/lang/Object;J)F");
            jmethodID viewGetContentOffsetY = env->GetStaticMethodID(isa,"viewGetContentOffsetY","(Ljava/lang/Object;J)F");

            p.x = env->CallStaticFloatMethod(isa,viewGetContentOffsetX,_view,(jlong) this);
            p.y = env->CallStaticFloatMethod(isa,viewGetContentOffsetY,_view,(jlong) this);

            env->DeleteLocalRef(isa);

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }

            return p;
        }

        kk::Strong<Canvas> OSView::createCanvas(Worker * worker) {

            if(worker == nullptr || worker->context() == nullptr) {
                return new OSCanvas(mainDispatchQueue(),_view);
            }

            kk::Strong<Worker> w = worker;
            kk::Weak<OSView> view = this;

            worker->context()->queue()->async([w,view]()->void{

                kk::Strong<OSView> vw = view.operator->();
                kk::Strong<Worker> v = w.operator->();

                if(v != nullptr && v->context() != nullptr && vw != nullptr) {

                    kk::Any c = new OSCanvas(v->context()->queue(),vw->_view);

                    JITContext::current()->forEach(v.get(), [&c](duk_context * ctx,void * heapptr)->void{

                        duk_push_heapptr(ctx, heapptr);

                        duk_get_prop_string(ctx, -1 ,"oncanvas");

                        if(duk_is_function(ctx, -1)) {

                            PushAny(ctx, c);

                            if(duk_pcall(ctx, 1) != DUK_EXEC_SUCCESS) {

                                kk::Error(ctx, -1, "[kk::ui::OSView::createCanvas] ");
                            }
                        }

                        duk_pop_2(ctx);

                    });

                }
            });

            return nullptr;

        }

        void OSView::addSubview(View * view,SubviewPosition position) {
            OSView * ov = dynamic_cast<OSView *>(view);
            if(ov == nullptr) {
                return ;
            }

            jboolean isAttach = false;

            JNIEnv *env = kk_env(&isAttach);

            jclass isa = env->FindClass("cn/kkmofang/ker/Native");

            jmethodID viewAddSubview = env->GetStaticMethodID(isa,"viewAddSubview","(Ljava/lang/Object;JLjava/lang/Object;I)V");

            env->CallStaticVoidMethod(isa,viewAddSubview,_view,(jlong) this,ov->object(),(jint)position);

            env->DeleteLocalRef(isa);

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }

            View::addSubview(view, position);
        }

        void OSView::removeView() {

            jboolean isAttach = false;

            JNIEnv *env = kk_env(&isAttach);

            jclass isa = env->FindClass("cn/kkmofang/ker/Native");

            jmethodID viewRemoveView = env->GetStaticMethodID(isa,"viewRemoveView","(Ljava/lang/Object;J)V");

            env->CallStaticVoidMethod(isa,viewRemoveView,_view,(jlong) this);

            env->DeleteLocalRef(isa);

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }

            View::removeView();
        }

        void OSView::removeAllSubviews() {

            jboolean isAttach = false;

            JNIEnv *env = kk_env(&isAttach);

            jclass isa = env->FindClass("cn/kkmofang/ker/Native");

            jmethodID viewRemoveView = env->GetStaticMethodID(isa,"viewRemoveView","(Ljava/lang/Object;J)V");

            auto i = _subviews.begin();

            while(i != _subviews.end()) {

                OSView * v = i->second;

                env->CallStaticVoidMethod(isa,viewRemoveView,v->_view,(jlong) v);

                i ++;
            }

            env->DeleteLocalRef(isa);

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }

        }

        void OSView::evaluateJavaScript(kk::CString code) {
            if(code == nullptr) {
                return;
            }

            jboolean isAttach = false;

            JNIEnv *env = kk_env(&isAttach);

            jclass isa = env->FindClass("cn/kkmofang/ker/Native");

            jmethodID viewEvaluateJavaScript = env->GetStaticMethodID(isa,"viewEvaluateJavaScript","(Ljava/lang/Object;JLjava/lang/String;)V");

            jstring v = env->NewStringUTF(code);

            env->CallStaticVoidMethod(isa,viewEvaluateJavaScript,_view,(jlong) this,v);

            env->DeleteLocalRef(v);
            env->DeleteLocalRef(isa);

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }
        }

        void OSView::setAttributedText(AttributedText * text) {

            jboolean isAttach = false;

            JNIEnv *env = kk_env(&isAttach);

            jclass isa = env->FindClass("cn/kkmofang/ker/Native");

            jmethodID viewSetAttributedText = env->GetStaticMethodID(isa,"viewSetAttributedText","(Ljava/lang/Object;JJ)V");

            env->CallStaticVoidMethod(isa,viewSetAttributedText,_view,(jlong) this,(jlong) text);

            env->DeleteLocalRef(isa);

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }
        }

        void OSView::doLoadImage() {
            if(_image != nullptr) {

                jboolean isAttach = false;

                JNIEnv *env = kk_env(&isAttach);

                jclass isa = env->FindClass("cn/kkmofang/ker/Native");

                jmethodID viewSetImage = env->GetStaticMethodID(isa,"viewSetImage","(Ljava/lang/Object;JLjava/lang/Object;)V");

                env->CallStaticVoidMethod(isa,viewSetImage,_view,(jlong) this,((OSImage *) _image)->object());

                env->DeleteLocalRef(isa);

                if(isAttach) {
                    gJavaVm->DetachCurrentThread();
                }

            }
        }

        void OSView::setImage(Image * image) {

            if(_image != nullptr && _onImageLoadFunc != nullptr) {
                _image->off("load", (kk::TFunction<void,kk::CString, Event *> *) _onImageLoadFunc);
            }

            View::setImage(image);

            jboolean isAttach = false;

            JNIEnv *env = kk_env(&isAttach);

            jclass isa = env->FindClass("cn/kkmofang/ker/Native");

            jmethodID viewSetImage = env->GetStaticMethodID(isa,"viewSetImage","(Ljava/lang/Object;JLjava/lang/Object;)V");

            if(image == nullptr) {
                env->CallStaticVoidMethod(isa,viewSetImage,_view,(jlong) this, nullptr);
            } else if(image->state() == ImageStateLoaded) {
                env->CallStaticVoidMethod(isa,viewSetImage,_view,(jlong) this, ((OSImage *) image)->object());
            } else if(image->state() != ImageStateError) {
                if(_onImageLoadFunc == nullptr) {
                    kk::Weak<OSView> v = this;
                    _onImageLoadFunc = new kk::TFunction<void, kk::CString,Event *>([v,image](kk::CString name,Event *event)->void{
                        kk::Strong<OSView> vv = v.operator->();
                        if(vv != nullptr && vv->_image == image) {
                            vv->doLoadImage();
                        }
                    });
                }
                _image->on("load", (kk::TFunction<void, kk::CString,Event *> *) _onImageLoadFunc);
            }

            env->DeleteLocalRef(isa);

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }


        }

        void OSView::setGravity(kk::CString gravity) {

            jboolean isAttach = false;

            JNIEnv *env = kk_env(&isAttach);

            jclass isa = env->FindClass("cn/kkmofang/ker/Native");

            jmethodID viewSetGravity = env->GetStaticMethodID(isa,"viewSetGravity","(Ljava/lang/Object;JLjava/lang/String;)V");

            jstring v = gravity == nullptr ? nullptr : env->NewStringUTF(gravity);

            env->CallStaticVoidMethod(isa,viewSetGravity,_view,(jlong) this,v);

            if(v != nullptr) {
                env->DeleteLocalRef(v);
            }

            env->DeleteLocalRef(isa);

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }

        }

        void OSView::setContent(kk::CString content,kk::CString contentType,kk::CString basePath) {

            jboolean isAttach = false;

            JNIEnv *env = kk_env(&isAttach);

            jclass isa = env->FindClass("cn/kkmofang/ker/Native");

            jmethodID viewSetContent = env->GetStaticMethodID(isa,"viewSetContent","(Ljava/lang/Object;JLjava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");

            jstring content_ = content == nullptr ? nullptr : env->NewStringUTF(content);
            jstring contentType_ = content == nullptr ? nullptr : env->NewStringUTF(contentType);
            jstring basePath_ = content == nullptr ? nullptr : env->NewStringUTF(basePath);

            env->CallStaticVoidMethod(isa,viewSetContent,_view,(jlong) this,content_,contentType_,basePath_);

            if(content_ != nullptr) {
                env->DeleteLocalRef(content_);
            }

            if(contentType_ != nullptr) {
                env->DeleteLocalRef(contentType_);
            }

            if(basePath_ != nullptr) {
                env->DeleteLocalRef(basePath_);
            }

            env->DeleteLocalRef(isa);

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }

        }

        jobject OSView::object() {
            return _view;
        }

        kk::Strong<View> createView(kk::CString name,ViewConfiguration * configuration,Context * context) {

            if(name == nullptr) {
                return nullptr;
            }

            kk::ui::KerApp * app = dynamic_cast<kk::ui::KerApp *>(context);

            if(app == nullptr) {
                return nullptr;
            }

            kk::Strong<View> v;

            jboolean isAttach = false;

            JNIEnv *env = kk_env(&isAttach);

            jclass isa = env->FindClass("cn/kkmofang/ker/Native");

            jmethodID createView = env->GetStaticMethodID(isa,"createView","(Lcn/kkmofang/ker/App;Ljava/lang/String;J)Ljava/lang/Object;");

            jstring name_ = env->NewStringUTF(name);

            jobject object = env->CallStaticObjectMethod(isa,createView,app->object(), name_,(jlong) configuration);

            if(object) {
                v = new OSView(object,configuration,context);
                env->DeleteLocalRef(object);
            }

            env->DeleteLocalRef(isa);

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }

            return v;
        }


        Size getAttributedTextContentSize(Context * context,AttributedText * text,kk::Float maxWidth) {
            return {};
        }

    }

}
