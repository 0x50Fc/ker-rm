//
// Created by zhanghailong on 2019/1/4.
//

#include "KerHttp.h"
#include <ui/ui.h>
#include <ui/app.h>
#include <ui/page.h>
#include <ui/package.h>
#include <ui/view.h>
#include <core/uri.h>
#include <core/http.h>
#include "global.h"
#include "kk.h"

namespace kk {

    void HTTPRequest::send(void * data,size_t size) {


        jboolean isAttach = false;

        JNIEnv *env = kk_env(&isAttach);

        jstring url = env->NewStringUTF(_url.c_str());
        jstring method = env->NewStringUTF(_method.c_str());
        jobject headers = env->NewObject(G.TreeMap.isa,G.TreeMap.init);
        jbyteArray body = nullptr;

        if(data && size > 0) {
            body =env->NewByteArray(size);
            env->SetByteArrayRegion(body,0,size, (jbyte *) data);
        }

        this->retain();

        jobject task = env->CallStaticObjectMethod(G.Http.isa,G.Http.send,(jlong) this,url,method,headers,body);

        if(task != nullptr) {
            setNative((kk::Native *) task);
            env->DeleteLocalRef(task);
        } else {
            this->release();
        }

        env->DeleteLocalRef(url);
        env->DeleteLocalRef(method);
        env->DeleteLocalRef(headers);

        if(body) {
            env->DeleteLocalRef(body);
        }

        if(isAttach) {
            gJavaVm->DetachCurrentThread();
        }


    }

    void HTTPRequest::cancel() {

        jobject task = (jobject) this->native();

        if(task) {

            jboolean isAttach = false;

            JNIEnv *env = kk_env(&isAttach);

            env->CallVoidMethod(task,G.SessionTask.cancel);

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }

            setNative(nullptr);

            this->release();

        }
    }

}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_KerHttp_onResponse(JNIEnv *env, jclass type, jlong id, jint statusCode,
                                        jobject headers) {

    kk::HTTPRequest * v = (kk::HTTPRequest *) id;

    if(headers != nullptr) {
        auto &m = v->respHeaders();
        jobject entrySet = env->CallObjectMethod(headers,G.Map.entrySet);
        jobject iterator = env->CallObjectMethod(entrySet,G.Collection.iterator);
        while(env->CallBooleanMethod(iterator,G.Iterator.hasNext)) {
            jobject entry = env->CallObjectMethod(iterator,G.Iterator.next);
            jstring key = (jstring) env->CallObjectMethod(entry,G.Map.Entry.getKey);
            jstring value = (jstring) env->CallObjectMethod(entry,G.Map.Entry.getValue);
            const char * skey = env->GetStringUTFChars(key,0);
            const char * svalue = env->GetStringUTFChars(value,0);
            m[skey] = svalue;
            env->ReleaseStringUTFChars(key,skey);
            env->ReleaseStringUTFChars(value,svalue);
            env->DeleteLocalRef(key);
            env->DeleteLocalRef(value);
            env->DeleteLocalRef(entry);
        }
        env->DeleteLocalRef(entrySet);
        env->DeleteLocalRef(iterator);
    }

    v->onResponse((kk::Int) statusCode);

}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_KerHttp_onData(JNIEnv *env, jclass type, jlong id, jbyteArray data_,
                                    jint offset, jint size) {
    jbyte *data = env->GetByteArrayElements(data_, NULL);

    kk::HTTPRequest * v = (kk::HTTPRequest *) id;

    if(data != nullptr) {
        v->onData(data + offset, size);
    }

    env->ReleaseByteArrayElements(data_, data, 0);
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_KerHttp_onError(JNIEnv *env, jclass type, jlong id, jstring errmsg_) {

    const char *errmsg = env->GetStringUTFChars(errmsg_, 0);

    kk::HTTPRequest * v = (kk::HTTPRequest *) id;

    kk::Weak<kk::HTTPRequest> w = v;

    v->onError(errmsg);
    v->queue()->async([w]()->void{
        kk::HTTPRequest * v = w.operator->();
        if(v != nullptr && v->native() != nullptr) {
            v->setNative(nullptr);
            v->release();
        }
    });

    env->ReleaseStringUTFChars(errmsg_, errmsg);
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_KerHttp_onDone(JNIEnv *env, jclass type, jlong id) {

    kk::HTTPRequest * v = (kk::HTTPRequest *) id;

    kk::Weak<kk::HTTPRequest> w = v;

    v->onDone();
    v->queue()->async([w]()->void{
        kk::HTTPRequest * v = w.operator->();
        if(v != nullptr && v->native() != nullptr) {
            v->setNative(nullptr);
            v->release();
        }
    });

}

