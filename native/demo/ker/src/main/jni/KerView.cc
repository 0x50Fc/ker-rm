//
// Created by hailong11 on 2018/12/12.
//

#include "KerView.h"
#include "kk.h"
#include "KerCanvas.h"
#include "KerImage.h"
#include "KerApp.h"
#include "global.h"

namespace kk {

    namespace ui {

        jobject getAttributedTextString(JNIEnv * env, AttributedText * text) {

            jobject v = env->NewObject(G.SpannableStringBuilder.isa,G.SpannableStringBuilder.init);

            if(text != nullptr) {
                auto& m = text->spans();
                auto i = m.begin();
                while(i != m.end()) {
                    AttributedTextSpan & span = *i;
                    if(span.type == AttributedTextSpanTypeText) {
                        jstring family = env->NewStringUTF(span.font.family.c_str());
                        jstring text = env->NewStringUTF(span.text.c_str());
                        env->CallStaticVoidMethod(G.Native.isa,
                                                  G.Native.appendText,
                                                  v,
                                                  text,
                                                  family,
                                                  (jfloat)span.font.size,
                                                  (jboolean)(span.font.weight == FontWeightBold),
                                                  (jboolean)(span.font.style == FontStyleItalic),
                                                  (jint)(span.color.intValue()));
                        env->DeleteLocalRef(family);
                        env->DeleteLocalRef(text);
                    } else if(span.type == AttributedTextSpanTypeImage) {
                        kk::ui::OSImage * image = span.image;
                        if(image != nullptr && image->object() != nullptr) {
                            env->CallStaticVoidMethod(G.Native.isa,
                                                      G.Native.appendImage,
                                                      v,
                                                      image->object(),
                                                      (jint)span.width,
                                                      (jint)span.height,
                                                      (jint)span.margin.left,
                                                      (jint)span.margin.top,
                                                      (jint)span.margin.right,
                                                      (jint)span.margin.bottom);
                        }
                    }
                    i ++;
                }
            }

            return v;
        }

        OSView::OSView(jobject view,ViewConfiguration * configuration,Context * context):View(configuration,context) {

            jboolean isAttach = false;

            JNIEnv *env = kk_env(&isAttach);

            _view = env->NewGlobalRef(view);

            env->CallStaticVoidMethod(G.Native.isa,G.Native.viewObtain,_view,(jlong) this);

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }

        }

        OSView::~OSView() {

            jboolean isAttach = false;

            JNIEnv *env = kk_env(&isAttach);

            env->CallStaticVoidMethod(G.Native.isa,G.Native.viewRecycle,_view,(jlong) this);

            env->DeleteGlobalRef(_view);

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }

            kk::Log("[OSView] [dealloc] 0x%x",(long) this);
        }

        void OSView::set(kk::CString name,kk::CString value) {

            jboolean isAttach = false;

            JNIEnv *env = kk_env(&isAttach);

            jstring name_ = env->NewStringUTF(name);
            jstring value_ = value == nullptr ? nullptr : env->NewStringUTF(value);

            env->CallStaticVoidMethod(G.Native.isa,G.Native.viewSetAttribute,_view,(jlong) this,name_,value_);

            env->DeleteLocalRef(name_);

            if(value_ != nullptr) {
                env->DeleteLocalRef(value_);
            }

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }

        }

        void OSView::setFrame(Rect & frame) {

            _frame = frame;

            jboolean isAttach = false;

            JNIEnv *env = kk_env(&isAttach);

            env->CallStaticVoidMethod(G.Native.isa,G.Native.viewSetFrame,_view,(jlong) this
                    ,(jfloat) frame.origin.x
                    ,(jfloat) frame.origin.y
                    ,(jfloat) frame.size.width
                    ,(jfloat) frame.size.height);

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }

        }

        void OSView::setContentSize(Size & size) {

            jboolean isAttach = false;

            JNIEnv *env = kk_env(&isAttach);

            env->CallStaticVoidMethod(G.Native.isa,G.Native.viewSetContentSize,_view,(jlong) this
                    ,(jfloat) size.width
                    ,(jfloat) size.height);

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }

        }

        void OSView::setContentOffset(Point & offset,kk::Boolean animated) {

            jboolean isAttach = false;

            JNIEnv *env = kk_env(&isAttach);

            env->CallStaticVoidMethod(G.Native.isa,G.Native.viewSetContentOffset,_view,(jlong) this
                    ,(jfloat) offset.x
                    ,(jfloat) offset.y
                    ,(jboolean) animated);

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }

        }

        Point OSView::contentOffset() {
            Point p;

            jboolean isAttach = false;

            JNIEnv *env = kk_env(&isAttach);

            p.x = env->CallStaticFloatMethod(G.Native.isa,G.Native.viewGetContentOffsetX,_view,(jlong) this);
            p.y = env->CallStaticFloatMethod(G.Native.isa,G.Native.viewGetContentOffsetY,_view,(jlong) this);

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }

            return p;
        }

        kk::Strong<Canvas> OSView::createCanvas(Worker * worker) {

            if(worker == nullptr || worker->context() == nullptr) {
                return new OSCanvas(mainDispatchQueue(),_view,(kk::Uint) _frame.size.width,(kk::Uint) _frame.size.height);
            }

            kk::Strong<Worker> w = worker;
            kk::Weak<OSView> view = this;
            kk::Uint width = (kk::Uint) _frame.size.width;
            kk::Uint height = (kk::Uint) _frame.size.height;

            worker->context()->queue()->async([w,view,width,height]()->void{

                kk::Strong<OSView> vw = view.operator->();
                kk::Strong<Worker> v = w.operator->();

                if(v != nullptr && v->context() != nullptr && vw != nullptr) {

                    kk::Any c = new OSCanvas(v->context()->queue(),vw->_view,width,height);

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

            env->CallStaticVoidMethod(G.Native.isa,G.Native.viewAddSubview,_view,(jlong) this,ov->object(),(jint)position);

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }

            View::addSubview(view, position);
        }

        void OSView::removeView() {

            jboolean isAttach = false;

            JNIEnv *env = kk_env(&isAttach);

            env->CallStaticVoidMethod(G.Native.isa,G.Native.viewRemoveView,_view,(jlong) this);

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }

            View::removeView();
        }

        void OSView::removeAllSubviews() {

            jboolean isAttach = false;

            JNIEnv *env = kk_env(&isAttach);

            auto i = _subviews.begin();

            while(i != _subviews.end()) {

                OSView * v = i->second;

                env->CallStaticVoidMethod(G.Native.isa,G.Native.viewRemoveView,v->_view,(jlong) v);

                i ++;
            }

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

            jstring v = env->NewStringUTF(code);

            env->CallStaticVoidMethod(G.Native.isa,G.Native.viewEvaluateJavaScript,_view,(jlong) this,v);

            env->DeleteLocalRef(v);

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }
        }

        void OSView::setAttributedText(AttributedText * text) {

            jboolean isAttach = false;

            JNIEnv *env = kk_env(&isAttach);

            jobject v = getAttributedTextString(env,text);

            env->CallStaticVoidMethod(G.Native.isa,G.Native.viewSetAttributedText,_view,(jlong) this,v);

            if(v != nullptr) {
                env->DeleteLocalRef(v);
            }

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }
        }

        void OSView::doLoadImage() {
            if(_image != nullptr) {

                jboolean isAttach = false;

                JNIEnv *env = kk_env(&isAttach);

                env->CallStaticVoidMethod(G.Native.isa,G.Native.viewSetImage,_view,(jlong) this,((OSImage *) _image)->object());

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

            if(image == nullptr) {
                env->CallStaticVoidMethod(G.Native.isa,G.Native.viewSetImage,_view,(jlong) this, nullptr);
            } else if(image->state() == ImageStateLoaded) {
                env->CallStaticVoidMethod(G.Native.isa,G.Native.viewSetImage,_view,(jlong) this, ((OSImage *) image)->object());
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

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }


        }

        void OSView::setGravity(kk::CString gravity) {

            jboolean isAttach = false;

            JNIEnv *env = kk_env(&isAttach);

            jstring v = gravity == nullptr ? nullptr : env->NewStringUTF(gravity);

            env->CallStaticVoidMethod(G.Native.isa,G.Native.viewSetGravity,_view,(jlong) this,v);

            if(v != nullptr) {
                env->DeleteLocalRef(v);
            }

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }

        }

        void OSView::setContent(kk::CString content,kk::CString contentType,kk::CString basePath) {

            jboolean isAttach = false;

            JNIEnv *env = kk_env(&isAttach);

            jstring content_ = content == nullptr ? nullptr : env->NewStringUTF(content);
            jstring contentType_ = content == nullptr ? nullptr : env->NewStringUTF(contentType);
            jstring basePath_ = content == nullptr ? nullptr : env->NewStringUTF(basePath);

            env->CallStaticVoidMethod(G.Native.isa,G.Native.viewSetContent,_view,(jlong) this,content_,contentType_,basePath_);

            if(content_ != nullptr) {
                env->DeleteLocalRef(content_);
            }

            if(contentType_ != nullptr) {
                env->DeleteLocalRef(contentType_);
            }

            if(basePath_ != nullptr) {
                env->DeleteLocalRef(basePath_);
            }

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

            jstring name_ = env->NewStringUTF(name);

            jobject object = env->CallStaticObjectMethod(G.Native.isa,G.Native.createView,app->object(), name_,(jlong) configuration);

            if(object) {
                v = new OSView(object,configuration,context);
                env->DeleteLocalRef(object);
            }

            env->DeleteLocalRef(name_);

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }

            return v;
        }


        Size getAttributedTextContentSize(Context * context,AttributedText * text,kk::Float maxWidth) {

            jboolean isAttach = false;

            JNIEnv *env = kk_env(&isAttach);

            jobject v = getAttributedTextString(env,text);

            jobject r = env->CallStaticObjectMethod(G.Native.isa,G.Native.getAttributedTextSize,v,(jfloat) maxWidth);

            if(v != nullptr) {
                env->DeleteLocalRef(v);
            }

            Size rr;

            if(r != nullptr) {

                jint n = env->GetArrayLength((jfloatArray) r);
                jfloat * p = env->GetFloatArrayElements((jfloatArray)r, nullptr);

                if(n > 0) {
                    rr.width = p[0];
                }

                if(n > 1) {
                    rr.height = p[1];
                }

                env->ReleaseFloatArrayElements((jfloatArray)r,p,0);

                env->DeleteLocalRef(r);

            }

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }

            return rr;
        }

    }

}
