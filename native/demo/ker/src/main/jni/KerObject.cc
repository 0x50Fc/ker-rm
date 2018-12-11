//
// Created by zhanghailong on 2016/11/7.
//

#include "kk.h"
#include "KerObject.h"
#include <core/kk.h>
#include <core/jit.h>

void duk_push_JObject(duk_context * ctx, jobject object) {

    jboolean isAttach = false;

    JNIEnv *env = kk_env(&isAttach);

    jclass isa = env->FindClass("cn/kkmofang/ker/Native");

    jmethodID pushObject = env->GetStaticMethodID(isa,"pushObject","(Ljava/lang/Object;)V");

    env->CallStaticVoidMethod(isa,pushObject,object);

    env->DeleteLocalRef(isa);

    if(isAttach) {
        gJavaVm->DetachCurrentThread();
    }

}

jobject duk_to_JObject(JNIEnv * env,duk_context * ctx, duk_idx_t idx) {

    jclass isa = env->FindClass("cn/kkmofang/ker/Native");

    jmethodID toObject = env->GetStaticMethodID(isa,"toObject","(JI)Ljava/lang/Object;");

    jobject v = env->CallStaticObjectMethod(isa,toObject,(jlong) ctx,(jint) idx);

    env->DeleteLocalRef(isa);

    return v;
}

namespace kk {

    void LogV(const char * format, va_list va) {
        kk_logv(format,va);
    }

    void PushNative(duk_context * ctx, kk::Native * native) {
        ::duk_push_JObject(ctx, (jobject) native);
    }

    NativeObject::NativeObject(Native * native):_native(nullptr) {

        if(native){

            jboolean isAttach = false;

            JNIEnv *env = kk_env(&isAttach);

            _native = (Native *) env->NewGlobalRef((jobject)native);

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }

        }
    }

    kk::String NativeObject::getPrototype(Native * native) {

        kk::String v ;

        jboolean isAttach = false;

        JNIEnv *env = kk_env(&isAttach);

        jclass isa = env->FindClass("cn/kkmofang/ker/Native");

        jmethodID getPrototype = env->GetStaticMethodID(isa,"getPrototype","(Ljava/lang/Object;)Ljava/lang/String;");

        jobject name =  env->CallStaticObjectMethod(isa,getPrototype,(jobject) native);

        if(name != nullptr) {

            kk::CString s = env->GetStringUTFChars((jstring) name, nullptr);

            v = s;

            env->ReleaseStringUTFChars((jstring)name,s);

            env->DeleteLocalRef(name);
        }

        env->DeleteLocalRef(isa);


        if(isAttach) {
            gJavaVm->DetachCurrentThread();
        }

        return v;

    }

    NativeObject::~NativeObject() {
        if(_native != nullptr) {

            jboolean isAttach = false;

            JNIEnv *env = kk_env(&isAttach);

            env->DeleteGlobalRef((jobject)_native);

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }

        }
    }

    Native * NativeObject::native() {
        return _native;
    }

    Any::operator Native*() {

        if(type == TypeObject) {
            {
                NativeObject * v = dynamic_cast<NativeObject *>(objectValue.get());
                if(v != nullptr) {
                    return v->native();
                }
            }
            {
                JSObject * v = dynamic_cast<JSObject *>(objectValue.get());

                if(v != nullptr) {

                    NativeObject * object = nullptr;

                    jboolean isAttach = false;

                    JNIEnv *env = kk_env(&isAttach);

                    jclass isa =env->FindClass("cn/kkmofang/ker/Native");

                    jmethodID allocJSObject = env->GetMethodID(isa,"allocJSObject","(J)Lcn/kkmofang/ker/JSObject;");

                    jobject native = env->CallStaticObjectMethod(isa,allocJSObject,(jlong) v);


                    if(native != nullptr) {
                        object = new kk::NativeObject((kk::Native *) native);
                        objectValue = object;
                        env->DeleteLocalRef(native);
                    }

                    env->DeleteLocalRef(isa);


                    if(isAttach) {
                        gJavaVm->DetachCurrentThread();
                    }

                    if(object != nullptr) {
                        return object->native();
                    }

                    return nullptr;
                }

            }
        }

        return nullptr;
    }

}
