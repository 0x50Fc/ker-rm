//
// Created by zhanghailong on 2019/1/3.
//

#include "KerUI.h"
#include <ui/ui.h>
#include <ui/app.h>
#include <ui/page.h>
#include <ui/package.h>
#include <ui/view.h>
#include <ui/ViewElement.h>
#include <ui/TextElement.h>
#include <core/uri.h>
#include <core/http.h>
#include "global.h"
#include "kk.h"
#include "KerObject.h"
#include "KerImage.h"
#include "KerView.h"
#include "KerCGContext.h"

namespace kk {


    kk::String GetDirectory(kk::CString name) {

        kk::String v;

        if(name != nullptr) {

            jboolean isAttach = false;

            JNIEnv * env = kk_env(&isAttach);

            jstring n = env->NewStringUTF(name);

            jstring s = (jstring) env->CallStaticObjectMethod(G.UI.isa,G.UI.getDirectory,n);

            env->DeleteLocalRef(n);

            if(s != nullptr) {
                kk::CString  c = env->GetStringUTFChars(s, nullptr);
                v = c;
                env->ReleaseStringUTFChars(s,c);
                env->DeleteLocalRef(s);
            }

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }

        }

        return v;
    }


    namespace ui {

        App::App(kk::Uint64 appid,kk::CString basePath,kk::CString appkey)
                :App(appid,basePath,"Android",G.UI.userAgent,appkey)
        {
        }

        void UI::commitTransaction() {

            jboolean isAttach = false;

            JNIEnv *env = kk_env(&isAttach);

            env->CallStaticVoidMethod(G.UI.isa,G.UI.commit);

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }


        }

        void UI::execCommand(App * app,Command * command) {
            startTransaction();

            {
                kk::ui::AppBackCommand * cmd = dynamic_cast<kk::ui::AppBackCommand *>(command);

                if(cmd) {

                    jboolean isAttach = false;

                    JNIEnv *env = kk_env(&isAttach);

                    env->CallStaticVoidMethod(G.UI.isa,G.UI.popPageViewController,(jint) cmd->delta,(jboolean) cmd->animated);

                    if(isAttach) {
                        gJavaVm->DetachCurrentThread();
                    }


                    return;
                }
            }


            {
                kk::ui::PageSetOptionsCommand * cmd = dynamic_cast<kk::ui::PageSetOptionsCommand *>(command);

                if(cmd) {

                    jboolean isAttach = false;

                    JNIEnv *env = kk_env(&isAttach);

                    jobject options = ker_Object_to_JObject(env,cmd->data);

                    env->CallStaticVoidMethod(G.UI.isa,G.UI.pageSetOptions,(jlong) cmd->pageId,options);

                    if(options) {
                        env->DeleteLocalRef(options);
                    }

                    if(isAttach) {
                        gJavaVm->DetachCurrentThread();
                    }

                    return;
                }
            }

            {
                kk::ui::PageCloseCommand * cmd = dynamic_cast<kk::ui::PageCloseCommand *>(command);

                if(cmd) {

                    jboolean isAttach = false;

                    JNIEnv *env = kk_env(&isAttach);

                    env->CallStaticVoidMethod(G.UI.isa,G.UI.pageClose,(jlong) cmd->pageId,(jboolean) cmd->animated);

                    if(isAttach) {
                        gJavaVm->DetachCurrentThread();
                    }

                    return;
                }
            }

            {
                kk::ui::ViewCreateCommand * v = dynamic_cast<kk::ui::ViewCreateCommand *>(command);

                if(v != nullptr) {

                    jboolean isAttach = false;

                    JNIEnv *env = kk_env(&isAttach);

                    jstring name = env->NewStringUTF(v->name.c_str());
                    jobject configuration = nullptr;

                    {
                        kk::ui::WebViewConfiguration * config = v->configuration;

                        if(config != nullptr) {

                            configuration = env->NewObject(G.WebViewConfiguration.isa,G.WebViewConfiguration.init);

                            {
                                auto & s = config->userScripts();

                                jobjectArray userScripts = env->NewObjectArray(s.size(),G.WebViewConfiguration.UserScript.isa, nullptr);

                                auto i = s.begin();

                                int idx = 0;

                                while(i != s.end()) {

                                    auto & u = *i;

                                    jstring source = env->NewStringUTF(u.code.c_str());

                                    jobject us = env->NewObject(G.WebViewConfiguration.UserScript.isa,G.WebViewConfiguration.UserScript.init);

                                    env->SetObjectField(us,G.WebViewConfiguration.UserScript.source,source);
                                    env->SetIntField(us,G.WebViewConfiguration.UserScript.type,u.injectionTime);

                                    env->SetObjectArrayElement(userScripts,idx,us);

                                    env->DeleteLocalRef(us);

                                    env->DeleteLocalRef(source);

                                    i ++;
                                    idx ++;
                                }

                                env->SetObjectField(configuration,G.WebViewConfiguration.userScripts,userScripts);

                                env->DeleteLocalRef(userScripts);

                            }

                            {

                                auto & a = config->userActions();

                                auto i = a.begin();
                                int idx = 0;

                                jobjectArray userActions = env->NewObjectArray(a.size(),G.WebViewConfiguration.UserAction.isa, nullptr);

                                while(i != a.end()) {

                                    auto & u = *i;

                                    jstring pattern = env->NewStringUTF(u.pattern.c_str());

                                    jobject us = env->NewObject(G.WebViewConfiguration.UserAction.isa,G.WebViewConfiguration.UserAction.init);

                                    env->SetObjectField(us,G.WebViewConfiguration.UserAction.pattern,pattern);
                                    env->SetIntField(us,G.WebViewConfiguration.UserAction.policy,u.policy);

                                    env->SetObjectArrayElement(userActions,idx,us);

                                    env->DeleteLocalRef(us);

                                    env->DeleteLocalRef(pattern);

                                    i ++;
                                    idx ++;
                                }

                                env->SetObjectField(configuration,G.WebViewConfiguration.userActions,userActions);

                                env->DeleteLocalRef(userActions);

                            }

                        }
                    }

                    env->CallStaticVoidMethod(G.UI.isa,G.UI.createView,name,configuration,(jlong) v->viewId,(jlong) app->appid());

                    if(isAttach) {
                        gJavaVm->DetachCurrentThread();
                    }

                    return;
                }
            }

            {
                kk::ui::ViewNativeCreateCommand * v = dynamic_cast<kk::ui::ViewNativeCreateCommand *>(command);

                if(v != nullptr) {

                    jboolean isAttach = false;

                    JNIEnv *env = kk_env(&isAttach);

                    env->CallStaticVoidMethod(G.UI.isa,G.UI.createNativeView, (jobject) v->native,(jlong) v->viewId,(jlong) app->appid());

                    if(isAttach) {
                        gJavaVm->DetachCurrentThread();
                    }

                    return;
                }
            }

            {
                kk::ui::ViewDeleteCommand * v = dynamic_cast<kk::ui::ViewDeleteCommand *>(command);

                if(v != nullptr) {

                    jboolean isAttach = false;

                    JNIEnv *env = kk_env(&isAttach);

                    env->CallStaticVoidMethod(G.UI.isa,G.UI.deleteView, (jlong) v->viewId, (jlong) app->appid());

                    if(isAttach) {
                        gJavaVm->DetachCurrentThread();
                    }

                    return;
                }
            }

            {
                kk::ui::ViewSetCommand * v = dynamic_cast<kk::ui::ViewSetCommand *>(command);

                if(v != nullptr) {

                    jboolean isAttach = false;

                    JNIEnv *env = kk_env(&isAttach);

                    jstring name = env->NewStringUTF(v->name.c_str());
                    jstring value = env->NewStringUTF(v->value.c_str());

                    env->CallStaticVoidMethod(G.UI.isa,G.UI.setAttribute, (jlong) v->viewId, name,value, (jlong) app->appid());

                    env->DeleteLocalRef(name);
                    env->DeleteLocalRef(value);

                    if(isAttach) {
                        gJavaVm->DetachCurrentThread();
                    }

                    return;
                }
            }

            {
                kk::ui::ViewSetFrameCommand * v = dynamic_cast<kk::ui::ViewSetFrameCommand *>(command);

                if(v != nullptr) {

                    jboolean isAttach = false;

                    JNIEnv *env = kk_env(&isAttach);

                    env->CallStaticVoidMethod(G.UI.isa,G.UI.setFrame, (jlong) v->viewId,
                                              (jint) v->frame.origin.x,(jint) v->frame.origin.y,
                                              (jint) v->frame.size.width,(jint) v->frame.size.height, (jlong) app->appid());


                    if(isAttach) {
                        gJavaVm->DetachCurrentThread();
                    }


                    return;
                }
            }

            {
                kk::ui::ViewAddsubviewCommand * v = dynamic_cast<kk::ui::ViewAddsubviewCommand *>(command);

                if(v != nullptr) {

                    jboolean isAttach = false;

                    JNIEnv *env = kk_env(&isAttach);

                    env->CallStaticVoidMethod(G.UI.isa,G.UI.addSubview, (jlong) v->viewId,(jlong) v->subviewId,(jint) v->position, (jlong) app->appid());

                    if(isAttach) {
                        gJavaVm->DetachCurrentThread();
                    }

                    return;
                }
            }

            {
                kk::ui::ViewRemoveCommand * v = dynamic_cast<kk::ui::ViewRemoveCommand *>(command);

                if(v != nullptr) {

                    jboolean isAttach = false;

                    JNIEnv *env = kk_env(&isAttach);

                    env->CallStaticVoidMethod(G.UI.isa,G.UI.removeView, (jlong) v->viewId, (jlong) app->appid());

                    if(isAttach) {
                        gJavaVm->DetachCurrentThread();
                    }


                    return;
                }
            }

            {
                kk::ui::ViewSetContentSizeCommand * v = dynamic_cast<kk::ui::ViewSetContentSizeCommand *>(command);

                if(v != nullptr) {

                    jboolean isAttach = false;

                    JNIEnv *env = kk_env(&isAttach);

                    env->CallStaticVoidMethod(G.UI.isa,G.UI.setContentSize, (jlong) v->viewId, (jint) v->size.width, (jint) v->size.height, (jlong) app->appid());

                    if(isAttach) {
                        gJavaVm->DetachCurrentThread();
                    }

                    return;
                }
            }

            {
                kk::ui::ViewSetContentOffsetCommand * v = dynamic_cast<kk::ui::ViewSetContentOffsetCommand *>(command);

                if(v != nullptr) {

                    jboolean isAttach = false;

                    JNIEnv *env = kk_env(&isAttach);

                    env->CallStaticVoidMethod(G.UI.isa,G.UI.setContentOffset, (jlong) v->viewId, (jint) v->offset.x, (jint) v->offset.y,(jboolean)v->animated, (jlong) app->appid());

                    if(isAttach) {
                        gJavaVm->DetachCurrentThread();
                    }

                    return;
                }
            }


            {
                kk::ui::ViewSetContentCommand * v = dynamic_cast<kk::ui::ViewSetContentCommand *>(command);

                if(v != nullptr) {

                    jboolean isAttach = false;

                    JNIEnv *env = kk_env(&isAttach);

                    jstring content = env->NewStringUTF(v->content.c_str());
                    jstring contentType = env->NewStringUTF(v->contentType.c_str());
                    jstring basePath = env->NewStringUTF(v->basePath.c_str());

                    env->CallStaticVoidMethod(G.UI.isa,G.UI.setContent, (jlong) v->viewId, content,contentType,basePath, (jlong) app->appid());

                    env->DeleteLocalRef(content);
                    env->DeleteLocalRef(contentType);
                    env->DeleteLocalRef(basePath);

                    if(isAttach) {
                        gJavaVm->DetachCurrentThread();
                    }

                    return;
                }
            }

            {
                kk::ui::ViewSetImageCommand * v = dynamic_cast<kk::ui::ViewSetImageCommand *>(command);

                if(v != nullptr) {

                    jboolean isAttach = false;

                    JNIEnv *env = kk_env(&isAttach);

                    jobject image = kk::ui::GetImageObject(v->image);

                    env->CallStaticVoidMethod(G.UI.isa,G.UI.setImage, (jlong) v->viewId,image, (jlong) app->appid());

                    if(isAttach) {
                        gJavaVm->DetachCurrentThread();
                    }

                    return;
                }
            }

            {
                kk::ui::ViewEvaluateJavaScriptCommand * v = dynamic_cast<kk::ui::ViewEvaluateJavaScriptCommand *>(command);

                if(v != nullptr) {

                    jboolean isAttach = false;

                    JNIEnv *env = kk_env(&isAttach);

                    jstring code = env->NewStringUTF(v->code.c_str());

                    env->CallStaticVoidMethod(G.UI.isa,G.UI.evaluateJavaScript, (jlong) v->viewId, code, (jlong) app->appid());

                    env->DeleteLocalRef(code);

                    if(isAttach) {
                        gJavaVm->DetachCurrentThread();
                    }

                    return;
                }
            }

            {
                kk::ui::ViewSetAttributedTextCommand * v = dynamic_cast<kk::ui::ViewSetAttributedTextCommand *>(command);

                if(v != nullptr) {

                    jboolean isAttach = false;

                    JNIEnv *env = kk_env(&isAttach);

                    jobject text = kk::ui::getAttributedTextString(env,v->text);

                    env->CallStaticVoidMethod(G.UI.isa,G.UI.setAttributedText, (jlong) v->viewId, text, (jlong) app->appid());

                    if(text) {
                        env->DeleteLocalRef(text);
                    }

                    if(isAttach) {
                        gJavaVm->DetachCurrentThread();
                    }

                    return;
                }
            }


            {
                kk::ui::CanvasDisplayCGContextCommand * cmd = dynamic_cast<kk::ui::CanvasDisplayCGContextCommand *>(command);

                if(cmd) {

                    kk::ui::displayCGContext(cmd->context,(jlong) cmd->viewId,(jlong) app->appid());

                    return ;
                }
            }

        }

        void Page::opening() {

            jboolean isAttach = false;

            JNIEnv * env = kk_env(&isAttach);

            this->retain();

            env->CallStaticVoidMethod(G.UI.isa,G.UI.createPage,(jlong) _pageId,(jlong) this);

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }

        }

        void Page::open(kk::Boolean animated) {

            jboolean isAttach = false;

            JNIEnv * env = kk_env(&isAttach);

            jstring type = env->NewStringUTF(_type.c_str());

            env->CallStaticVoidMethod(G.UI.isa,G.UI.openPageViewController,(jlong) _pageId,(jboolean) animated,type);

            env->DeleteLocalRef(type);

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }

        }

    }
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_KerUI_setImage(JNIEnv *env, jclass type, jlong id, jobject image, jint width,
                                    jint height) {

    kk::ui::NativeImage * i = (kk::ui::NativeImage *) id;

    kk::Strong<kk::NativeObject> v ;

    if(image != nullptr) {
        v = new kk::NativeObject((kk::Native *) image);
    }

    i->queue()->async([v,i,width,height]()->void{

        if(v.operator->() == nullptr) {
            i->setState(kk::ui::ImageStateError);
        } else {
            i->setNative(v.operator->()->native(),(kk::Uint) width,(kk::Uint) height);
            i->setState(kk::ui::ImageStateLoaded);
        }

        i->release();
    });


}


extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_KerUI_emit(JNIEnv *env, jclass type, jlong appid, jstring name_, jlong viewId,
                                jobject data) {
    const char *name = env->GetStringUTFChars(name_, 0);

    kk::Strong<kk::Event> e = new kk::Event();

    if(data != nullptr) {
        e->setData(new kk::NativeValue((kk::Native *) data));
    }

    kk::ui::UI::main()->emit(appid,viewId, name, e);

    env->ReleaseStringUTFChars(name_, name);
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_KerUI_dispatchSetContentOffset(JNIEnv *env, jclass type, jlong appid, jint x,
                                                    jint y, jlong viewId) {

    kk::Strong<kk::ui::ViewSetContentOffsetCommand> cmd = new kk::ui::ViewSetContentOffsetCommand();
    cmd->viewId = viewId;
    cmd->offset.x = x;
    cmd->offset.y = y;
    kk::ui::UI::main()->dispatchCommand(appid, cmd);

}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_JSObject_retain(JNIEnv *env, jclass type, jlong ptr) {

    kk::JSObject * v = (kk::JSObject *) ptr;
    v->retain();
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_JSObject_release(JNIEnv *env, jclass type, jlong ptr) {

    kk::JSObject * v = (kk::JSObject *) ptr;
    v->queue()->async([v]()->void{
        v->release();
    });
}

extern "C"
JNIEXPORT jobject JNICALL
Java_cn_kkmofang_ker_JSObject_get(JNIEnv *env, jclass type, jlong ptr,
                                                        jstring key_) {
    const char *key = env->GetStringUTFChars(key_, 0);

    kk::JSObject * v = (kk::JSObject *) ptr;
    jobject r = nullptr;

    v->queue()->sync([key,&r,v]()->void{

        duk_context * ctx = v->jsContext();
        void * heapptr = v->heapptr();

        if(ctx && heapptr) {
            duk_push_heapptr(ctx,heapptr);
            duk_get_prop_string(ctx,-1,key);

            jboolean isAttach = false;

            JNIEnv * env = kk_env(&isAttach);

            jobject vv = duk_to_JObject(env,ctx,-1);

            if(vv) {
                r = env->NewGlobalRef(vv);
                env->DeleteLocalRef(vv);
            }

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }

            duk_pop_2(ctx);
        }

    });

    env->ReleaseStringUTFChars(key_, key);

    if(r != nullptr) {
        jobject g = r;
        r = env->NewLocalRef(r);
        env->DeleteGlobalRef(g);
    }

    return r;
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_JSObject_set(JNIEnv *env, jclass type,
                                                                           jlong ptr, jstring key_,
                                                                           jobject value) {
    const char *key = env->GetStringUTFChars(key_, 0);

    kk::JSObject * v = (kk::JSObject *) ptr;

    jobject vv = nullptr;

    if(value) {
        vv = env->NewGlobalRef(value);
    }

    v->queue()->sync([key,v,vv]()->void{

        duk_context * ctx = v->jsContext();
        void * heapptr = v->heapptr();

        if(ctx && heapptr) {
            duk_push_heapptr(ctx,heapptr);
            duk_push_string(ctx,key);
            duk_push_JObject(ctx,vv);
            duk_put_prop(ctx,-3);
            duk_pop(ctx);
        }

    });

    if(vv) {
        env->DeleteGlobalRef(vv);
    }

    env->ReleaseStringUTFChars(key_, key);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_cn_kkmofang_ker_JSObject_call(JNIEnv *env, jclass type, jlong ptr,
                                                               jobjectArray args) {

    kk::JSObject * v = (kk::JSObject *) ptr;

    jobject r = nullptr;
    jobject a = args;

    if(a) {
        a = env->NewGlobalRef(a);
    }

    v->queue()->sync([v,a,&r]()->void{

        duk_context * ctx = v->jsContext();
        void * heapptr = v->heapptr();

        if(ctx && heapptr) {
            duk_push_heapptr(ctx,heapptr);
            if(duk_is_function(ctx,-1)) {

                jboolean isAttach = false;

                JNIEnv * env = kk_env(&isAttach);

                duk_idx_t n = 0;

                if(a != nullptr) {
                    n = env->GetArrayLength((jobjectArray)a);
                    for(duk_idx_t i= 0;i<n;i++) {
                        jobject v = env->GetObjectArrayElement((jobjectArray)a,i);
                        duk_push_JObject(ctx,v);
                        if(v != nullptr) {
                            env->DeleteLocalRef(v);
                        }
                    }
                }

                if(duk_pcall(ctx,n) != DUK_EXEC_SUCCESS) {
                    kk::Error(ctx,-1,"[JSObject] [call] ");
                } else {
                    jobject rr = duk_to_JObject(env,ctx,-1);
                    if(rr) {
                        r = env->NewGlobalRef(rr);
                        env->DeleteLocalRef(rr);
                    }
                }

                if(isAttach) {
                    gJavaVm->DetachCurrentThread();
                }


            }
            duk_pop(ctx);
        }

    });

    if(a) {
        env->DeleteGlobalRef(a);
    }

    if(r) {
        jobject vv = env->NewLocalRef(r);
        env->DeleteGlobalRef(r);
        r = vv;
    }

    return r;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_cn_kkmofang_ker_JSObject_invoke(JNIEnv *env,
                                                                                    jclass type,
                                                                                    jlong ptr,
                                                                                    jstring name_,
                                                                                    jobjectArray args) {
    const char *name = env->GetStringUTFChars(name_, 0);

    kk::JSObject * v = (kk::JSObject *) ptr;

    jobject r = nullptr;
    jobject a = args;

    if(a) {
        a = env->NewGlobalRef(a);
    }

    v->queue()->sync([v,a,&r,name]()->void{

        duk_context * ctx = v->jsContext();
        void * heapptr = v->heapptr();

        if(ctx && heapptr) {
            duk_push_heapptr(ctx,heapptr);
            duk_get_prop_string(ctx,-1,name);

            if(duk_is_function(ctx,-1)) {

                duk_dup(ctx,-2);

                jboolean isAttach = false;

                JNIEnv * env = kk_env(&isAttach);

                duk_idx_t n = 0;

                if(a != nullptr) {
                    n = env->GetArrayLength((jobjectArray)a);
                    for(duk_idx_t i= 0;i<n;i++) {
                        jobject v = env->GetObjectArrayElement((jobjectArray)a,i);
                        duk_push_JObject(ctx,v);
                        if(v != nullptr) {
                            env->DeleteLocalRef(v);
                        }
                    }
                }

                if(duk_pcall_method(ctx,n) != DUK_EXEC_SUCCESS) {
                    kk::Error(ctx,-1,"[JSObject] [call] ");
                } else {
                    jobject rr = duk_to_JObject(env,ctx,-1);
                    if(rr) {
                        r = env->NewGlobalRef(rr);
                        env->DeleteLocalRef(rr);
                    }
                }

                if(isAttach) {
                    gJavaVm->DetachCurrentThread();
                }


            }

            duk_pop_2(ctx);
        }

    });

    if(a) {
        env->DeleteGlobalRef(a);
    }

    if(r) {
        jobject vv = env->NewLocalRef(r);
        env->DeleteGlobalRef(r);
        r = vv;
    }

    env->ReleaseStringUTFChars(name_, name);

    return r;

}

struct KerProperty {
    kk::String name;
    kk::String type;
    kk::Boolean readonly;
    jfieldID field;
};

struct KerMethod {
    kk::String name;
    kk::String type;
    jmethodID method;
};

struct KerPrototype {
    kk::String name;
    kk::String base;
    std::vector<KerProperty> propertys;
    std::vector<KerMethod> methods;
};

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_KerUI_addPrototype__JLjava_lang_String_2Ljava_lang_String_2_3Ljava_lang_Object_2_3Ljava_lang_Object_2(
        JNIEnv *env, jclass type, jlong appid, jstring name_, jstring base_, jobjectArray methods,
        jobjectArray fields) {

    const char *name =  env->GetStringUTFChars(name_, 0);

    const char *base = base_ == nullptr ? nullptr :env->GetStringUTFChars(base_, 0);

    KerPrototype p;

    p.name = name;

    if(base) {
        p.base = base;
    }

    if(methods != nullptr) {
        jsize n = env->GetArrayLength(methods);
        for(jsize i=0;i + 2 <n;i+=3) {
            {
                jstring name = (jstring) env->GetObjectArrayElement(methods,i);
                jstring type = (jstring) env->GetObjectArrayElement(methods,i + 1);
                jobject method = env->GetObjectArrayElement(methods,i + 2);
                KerMethod m;
                {
                    const char * v = env->GetStringUTFChars(name,0);
                    m.name = v;
                    env->ReleaseStringUTFChars(name,v);
                    env->DeleteLocalRef(name);
                }
                {
                    const char * v = env->GetStringUTFChars(type,0);
                    m.type = v;
                    env->ReleaseStringUTFChars(type,v);
                    env->DeleteLocalRef(type);
                }
                m.method = env->FromReflectedMethod(method);
                env->DeleteLocalRef(method);
                p.methods.push_back(m);
            }
        }
    }

    if(fields != nullptr) {
        jsize n = env->GetArrayLength(fields);
        for(jsize i=0;i + 3 <n;i+=4) {

            {
                jstring name = (jstring)env->GetObjectArrayElement(fields,i);
                jstring type = (jstring) env->GetObjectArrayElement(fields,i + 1);
                jobject readonly = env->GetObjectArrayElement(fields,i + 2);
                jobject field = env->GetObjectArrayElement(fields,i + 3);
                KerProperty prop;
                {
                    const char * v = env->GetStringUTFChars(name,0);
                    prop.name = v;
                    env->ReleaseStringUTFChars(name,v);
                    env->DeleteLocalRef(name);
                }
                {
                    const char * v = env->GetStringUTFChars(type,0);
                    prop.type = v;
                    env->ReleaseStringUTFChars(type,v);
                    env->DeleteLocalRef(type);
                }
                {
                    prop.readonly = env->CallBooleanMethod(readonly,G.Boolean.booleanValue);
                }
                {
                    prop.field = env->FromReflectedField(field);
                    env->DeleteLocalRef(field);
                }
                p.propertys.push_back(prop);
            }

        }
    }

    kk::Strong<kk::ui::App> app = kk::ui::UI::main()->getApp(appid);

    if(app != nullptr) {

        duk_context * ctx = app->jsContext();

        duk_push_c_function(ctx,[](duk_context * ctx)->duk_ret_t{
            return 0;
        },0);

        duk_push_object(ctx);

        if(!p.base.empty()) {
            kk::SetPrototype(ctx,-1,p.base.c_str());
        }

        {
            auto i = p.propertys.begin();
            auto e = p.propertys.end();
            while(i != e) {

                const KerProperty & prop = * i;

                duk_push_string(ctx,prop.name.c_str());

                duk_push_c_function(ctx,[](duk_context * ctx)->duk_ret_t{

                    duk_push_this(ctx);

                    kk::NativeObject * object = dynamic_cast<kk::NativeObject *>(kk::GetObject(ctx,-1));

                    duk_pop(ctx);

                    duk_push_current_function(ctx);

                    duk_get_prop_string(ctx,-1,"__field");

                    jfieldID fd = (jfieldID) duk_to_pointer(ctx,-1);

                    duk_pop(ctx);

                    duk_get_prop_string(ctx,-1,"__type");

                    kk::CString type = duk_to_string(ctx,-1);

                    duk_pop(ctx);

                    duk_pop(ctx);

                    if(fd && type && object) {

                        jboolean isAttach = false;

                        JNIEnv *env = kk_env(&isAttach);

                        duk_ret_t r = JObjectGetProperty(env,(jobject) object->native(),fd,type,ctx);

                        if(isAttach) {
                            gJavaVm->DetachCurrentThread();
                        }

                        return r;
                    }

                    return 0;
                },0);

                duk_push_pointer(ctx,prop.field);
                duk_put_prop_string(ctx,-2,"__field");

                duk_push_string(ctx,prop.type.c_str());
                duk_put_prop_string(ctx,-2,"__type");

                if(prop.readonly) {
                    duk_def_prop(ctx,-3,DUK_DEFPROP_SET_CONFIGURABLE | DUK_DEFPROP_SET_ENUMERABLE | DUK_DEFPROP_HAVE_GETTER);
                } else {

                    duk_push_c_function(ctx,[](duk_context * ctx)->duk_ret_t{

                        duk_push_this(ctx);

                        kk::NativeObject * object = dynamic_cast<kk::NativeObject *>(kk::GetObject(ctx,-1));

                        duk_pop(ctx);

                        duk_push_current_function(ctx);

                        duk_get_prop_string(ctx,-1,"__field");

                        jfieldID fd = (jfieldID) duk_to_pointer(ctx,-1);

                        duk_pop(ctx);

                        duk_get_prop_string(ctx,-1,"__type");

                        kk::CString type = duk_to_string(ctx,-1);

                        duk_pop(ctx);

                        duk_pop(ctx);

                        if(fd && type && object) {

                            jboolean isAttach = false;

                            JNIEnv *env = kk_env(&isAttach);

                            duk_ret_t r = JObjectSetProperty(env,(jobject) object->native(),fd,type,ctx);

                            {
                                jclass isa = env->FindClass("cn/kkmofang/ker/Native");
                                jmethodID gc = env->GetStaticMethodID(isa,"gc","()V");
                                env->CallStaticVoidMethod(isa,gc);
                                env->DeleteLocalRef(isa);
                            }

                            if(isAttach) {
                                gJavaVm->DetachCurrentThread();
                            }

                            return r;
                        }

                        return 0;
                    },0);

                    duk_push_pointer(ctx,prop.field);
                    duk_put_prop_string(ctx,-2,"__field");

                    duk_push_string(ctx,prop.type.c_str());
                    duk_put_prop_string(ctx,-2,"__type");

                    duk_def_prop(ctx,-4,DUK_DEFPROP_SET_CONFIGURABLE | DUK_DEFPROP_SET_ENUMERABLE | DUK_DEFPROP_HAVE_GETTER | DUK_DEFPROP_HAVE_SETTER);
                }

                i ++;
            }
        }

        {
            auto i = p.methods.begin();
            auto e = p.methods.end();
            while(i != e) {

                const KerMethod & method = * i;

                duk_push_string(ctx,method.name.c_str());

                duk_push_c_function(ctx,[](duk_context * ctx)->duk_ret_t{

                    duk_push_this(ctx);

                    kk::NativeObject * object = dynamic_cast<kk::NativeObject *>(kk::GetObject(ctx,-1));

                    duk_pop(ctx);

                    duk_push_current_function(ctx);

                    duk_get_prop_string(ctx,-1,"__method");

                    jmethodID m = (jmethodID) duk_to_pointer(ctx,-1);

                    duk_pop(ctx);

                    duk_get_prop_string(ctx,-1,"__type");

                    kk::CString type = duk_to_string(ctx,-1);

                    duk_pop(ctx);

                    duk_pop(ctx);

                    if(m && type && object) {

                        jboolean isAttach = false;

                        JNIEnv *env = kk_env(&isAttach);

                        duk_ret_t r = JObjectInvoke(env,(jobject) object->native(),m,type,ctx,duk_get_top(ctx));

                        if(isAttach) {
                            gJavaVm->DetachCurrentThread();
                        }

                        return r;
                    }

                    return 0;
                }, JObjectMethodTypeArgumentCount(method.type.c_str()));

                duk_push_pointer(ctx,method.method);
                duk_put_prop_string(ctx,-2,"__method");

                duk_push_string(ctx,method.type.c_str());
                duk_put_prop_string(ctx,-2,"__type");

                duk_def_prop(ctx,-3,DUK_DEFPROP_SET_CONFIGURABLE | DUK_DEFPROP_SET_ENUMERABLE | DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_CLEAR_WRITABLE );


                i ++;
            }
        }

        duk_set_prototype(ctx,-2);
        duk_put_global_string(ctx,p.name.c_str());

    }

    // TODO

    env->ReleaseStringUTFChars(name_, name);

    if(base) {
        env->ReleaseStringUTFChars(base_, base);
    }

}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_KerUI_addLibrary(JNIEnv *env, jclass type, jlong appid, jstring name_,
                                      jobject object) {
    const char *name = env->GetStringUTFChars(name_, 0);

    if(object != nullptr) {

        kk::Strong<kk::ui::App> app = kk::ui::UI::main()->getApp(appid);

        if(app != nullptr) {

            duk_context * ctx = app->jsContext();

            duk_push_JObject(ctx,object);

            duk_put_global_string(ctx,name);

        }

    }


    env->ReleaseStringUTFChars(name_, name);
}


extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_KerUI_openlib__(JNIEnv *env, jclass type) {

    kk::ui::App::Openlib();

    kk::ui::addAppOpenlib([](duk_context * ctx, kk::ui::App * app)->void{

        jboolean isAttach = false;

        JNIEnv *env = kk_env(&isAttach);

        env->CallStaticVoidMethod(G.UI.isa,G.UI.openlib,(jlong) app->appid());

        if(isAttach) {
            gJavaVm->DetachCurrentThread();
        }


    });
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_KerUI_open(JNIEnv *env, jclass type, jstring URI_, jobject query,
                                jobject callback) {
    const char *URI = env->GetStringUTFChars(URI_, 0);

    kk::Strong<kk::NativeObject> cb = new kk::NativeObject((kk::Native *) callback);
    kk::Strong<kk::NativeValue> q = new kk::NativeValue((kk::Native *) query);

    kk::ui::UI::main()->open(URI, q,[cb](kk::Uint64 appid,kk::CString errmsg)->void{

        jobject callback = (jobject) cb.operator->()->native();

        if(callback != nullptr) {

            jboolean isAttach = false;

            JNIEnv *env = kk_env(&isAttach);

            if(errmsg != nullptr) {
                jstring err = env->NewStringUTF(errmsg);
                env->CallVoidMethod(callback,G.UI.OpenCallback.onError,err);
                env->DeleteLocalRef(err);
            } else {
                env->CallVoidMethod(callback,G.UI.OpenCallback.onOpen,(jlong) appid);
            }

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }

        }

    });

    env->ReleaseStringUTFChars(URI_, URI);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_cn_kkmofang_ker_KerUI_resolveURI__Ljava_lang_String_2(JNIEnv *env, jclass type,
                                                           jstring path_) {
    const char *path = env->GetStringUTFChars(path_, 0);

    kk::String v = kk::ResolveURI(path);

    env->ReleaseStringUTFChars(path_, path);

    return env->NewStringUTF(v.c_str());
}
extern "C"
JNIEXPORT jstring JNICALL
Java_cn_kkmofang_ker_KerUI_resolveURI__Ljava_lang_String_2Ljava_lang_String_2(JNIEnv *env,
                                                                              jclass type,
                                                                              jstring path_,
                                                                              jstring basePath_) {
    const char *path = env->GetStringUTFChars(path_, 0);
    const char *basePath = env->GetStringUTFChars(basePath_, 0);

    kk::String v;

    if(kk::CStringHasSubstring(path,"://")) {
        v = kk::ResolveURI(path);
    } else {
        kk::String p = basePath;
        if(!kk::CStringHasSuffix(p.c_str(),"/")) {
            p.append("/");
        }
        p.append(path);
        v = kk::ResolveURI(p.c_str());
    }

    env->ReleaseStringUTFChars(path_, path);
    env->ReleaseStringUTFChars(basePath_, basePath);

    return env->NewStringUTF(v.c_str());
}

extern "C"
JNIEXPORT jstring JNICALL
Java_cn_kkmofang_ker_KerUI_resolveURI__Ljava_lang_String_2J(JNIEnv *env, jclass type, jstring path_,
                                                            jlong appid) {
    const char *path = env->GetStringUTFChars(path_, 0);

    kk::String v ;

    if(kk::CStringHasSubstring(path,"://")) {
        v = kk::ResolveURI(path);
    } else {
        kk::String p = path;
        kk::ui::UI::main()->queue()->sync([appid,&p]()->void{
            kk::Strong<kk::ui::App> app = kk::ui::UI::main()->getApp(appid);
            if(app != nullptr) {
                p = app->absolutePath(p.c_str());
            }
        });
        v = kk::ResolveURI(p.c_str());
    }

    env->ReleaseStringUTFChars(path_, path);

    return env->NewStringUTF(v.c_str());
}

extern "C"
JNIEXPORT jstring JNICALL
Java_cn_kkmofang_ker_KerUI_resolvePath__Ljava_lang_String_2(JNIEnv *env, jclass type,
                                                            jstring URI_) {
    const char *URI = env->GetStringUTFChars(URI_, 0);

    kk::String v = kk::ResolvePath(URI);

    env->ReleaseStringUTFChars(URI_, URI);

    return env->NewStringUTF(v.c_str());
}

extern "C"
JNIEXPORT jstring JNICALL
Java_cn_kkmofang_ker_KerUI_resolvePath__Ljava_lang_String_2Ljava_lang_String_2(JNIEnv *env,
                                                                               jclass type,
                                                                               jstring URI_,
                                                                               jstring basePath_) {
    const char *URI = env->GetStringUTFChars(URI_, 0);
    const char *basePath = env->GetStringUTFChars(basePath_, 0);

    kk::String v;

    if(kk::CStringHasSubstring(URI,"://")) {
        v = kk::ResolvePath(URI);
    } else {
        kk::String p = basePath;
        if(!kk::CStringHasSuffix(p.c_str(),"/")) {
            p.append("/");
        }
        p.append(URI);
        v = kk::ResolvePath(p.c_str());
    }

    env->ReleaseStringUTFChars(URI_, URI);
    env->ReleaseStringUTFChars(basePath_, basePath);

    return env->NewStringUTF(v.c_str());
}

extern "C"
JNIEXPORT jstring JNICALL
Java_cn_kkmofang_ker_KerUI_resolvePath__Ljava_lang_String_2J(JNIEnv *env, jclass type, jstring URI_,
                                                             jlong appid) {
    const char *URI = env->GetStringUTFChars(URI_, 0);

    kk::String v ;

    if(kk::CStringHasSubstring(URI,"://")) {
        v = kk::ResolvePath(URI);
    } else {
        kk::String p = URI;
        kk::ui::UI::main()->queue()->sync([appid,&p]()->void{
            kk::Strong<kk::ui::App> app = kk::ui::UI::main()->getApp(appid);
            if(app != nullptr) {
                p = app->absolutePath(p.c_str());
            }
        });
        v = kk::ResolvePath(p.c_str());
    }

    env->ReleaseStringUTFChars(URI_, URI);

    return env->NewStringUTF(v.c_str());
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_KerUI_addViewClass__Ljava_lang_String_2(JNIEnv *env, jclass type,
                                                             jstring name_) {
    const char *name = env->GetStringUTFChars(name_, 0);

    kk::ui::ViewElement::library(name);

    env->ReleaseStringUTFChars(name_, name);
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_KerUI_addTextViewClass__Ljava_lang_String_2(JNIEnv *env, jclass type,
                                                                 jstring name_) {
    const char *name = env->GetStringUTFChars(name_, 0);

    kk::ui::TextElement::library(name);

    env->ReleaseStringUTFChars(name_, name);
}