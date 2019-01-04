//
// Created by zhanghailong on 2019/1/4.
//

#include "KerPage.h"
#include <ui/ui.h>
#include <ui/app.h>
#include <ui/page.h>
#include <ui/package.h>
#include <ui/view.h>
#include <core/uri.h>
#include <core/http.h>
#include "global.h"
#include "kk.h"
#include "KerObject.h"

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_Page_dealloc(JNIEnv *env, jclass type, jlong ptr) {


    kk::ui::Page * page =  (kk::ui::Page *) ptr;

    page->queue()->async([page]()->void{
        page->release();
    });

}


extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_Page_open__JLandroid_view_View_2II(JNIEnv *env, jclass type, jlong ptr,
                                                        jobject view, jint width, jint height) {

    kk::Weak<kk::ui::Page> p =  (kk::ui::Page *) ptr;
    kk::Strong<kk::NativeObject> vv = new kk::NativeObject((kk::Native *) view);
    kk::ui::Rect r = { {0,0} ,{(kk::ui::Float) width, (kk::ui::Float) height} };

    p->queue()->async([r,p,vv]()->void{
        kk::Strong<kk::ui::Page> page = p.operator->();
        if(page !=nullptr) {
            kk::Strong<kk::ui::View> v = page->app()->createView(vv.operator->()->native(), (kk::ui::Rect &) r);
            page->setView(v);
            page->setSize((kk::ui::Size &) r.size);
            page->ready();
        }
    });

}


extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_Page_setSize(JNIEnv *env, jclass type, jlong ptr, jint width, jint height) {

    kk::Weak<kk::ui::Page> p =  (kk::ui::Page *) ptr;
    kk::ui::Size s = {(kk::ui::Float) width, (kk::ui::Float) height};
    p->queue()->async([s,p]()->void{
        kk::Strong<kk::ui::Page> page = p.operator->();
        if(page !=nullptr) {
            page->setSize((kk::ui::Size&) s);
        }
    });
}


extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_Page_addLibrary__JLjava_lang_String_2Ljava_lang_Object_2(JNIEnv *env,
                                                                              jclass type,
                                                                              jlong ptr,
                                                                              jstring name_,
                                                                              jobject object) {
    const char *name = env->GetStringUTFChars(name_, 0);

    kk::ui::Page * page = (kk::ui::Page *) ptr;

    kk::Any v = new kk::NativeObject((kk::Native *) object);

    page->addLibrary(name,v);

    env->ReleaseStringUTFChars(name_, name);

}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_Page_emit__JLjava_lang_String_2Ljava_lang_Object_2(JNIEnv *env, jclass type,
                                                                        jlong ptr, jstring name_,
                                                                        jobject data) {
    const char *name = env->GetStringUTFChars(name_, 0);

    kk::Weak<kk::ui::Page> p = (kk::ui::Page *) ptr;

    kk::String n = name;
    kk::Strong<kk::NativeObject> v = new kk::NativeObject((kk::Native *) data);

    p->queue()->async([n,v,p]()->void{
        kk::Strong<kk::ui::Page> page = p.operator->();
        if(page != nullptr) {
            kk::Strong<kk::Event> e = new kk::Event();
            e->setData(v.operator->());
            page->emit(n.c_str(),e);
        }
    });

    env->ReleaseStringUTFChars(name_, name);
}

