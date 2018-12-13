#include <jni.h>
#include <ui/app.h>
#include "kk.h"
#include "KerImage.h"
#include "KerObject.h"
#include "KerApp.h"
#include <core/dispatch.h>
#include <event.h>
#include "KerPackage.h"

namespace kk {
    extern event_base * GetDispatchQueueEventBase(DispatchQueue * queue);
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_Native_retain(JNIEnv *env, jclass type, jlong kerObject) {

    kk::Object * object = (kk::Object *) kerObject;

    if(object) {
        object->retain();
    }

}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_Native_release(JNIEnv *env, jclass type, jlong kerObject) {

    kk::Object * object = (kk::Object *) kerObject;

    if(object) {
        object->release();
    }

}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_JSContext_PushFunction(JNIEnv *env, jclass type, jlong jsContext,
                                            jobject func) {

    duk_context * ctx = (duk_context *) jsContext;

    if(func == nullptr) {

        duk_push_undefined(ctx);

    } else {

        duk_push_c_function(ctx,[](duk_context * ctx)->duk_ret_t {

            duk_push_current_function(ctx);
            duk_get_prop_string(ctx,-1,"__func");
            kk::NativeObject * v = dynamic_cast<kk::NativeObject *>(kk::GetObject(ctx,-1));
            duk_pop_2(ctx);

            if(v) {

                jobject object = (jobject) v->native();

                jboolean isAttach = false;

                JNIEnv * env = kk_env(&isAttach);

                jclass isa = env->FindClass("cn/kkmofang/ker/JSFunction");

                jmethodID invoke = env->GetMethodID(isa,"invoke","(J)I");

                duk_ret_t r = env->CallIntMethod(object,invoke,(jlong) ctx);

                env->DeleteLocalRef(isa);

                if(isAttach) {
                    gJavaVm->DetachCurrentThread();
                }

                return r;

            }

            return 0;

        },DUK_VARARGS);

        kk::PushObject(ctx,new kk::NativeObject((kk::Native *) func));
        duk_put_prop_string(ctx,-2,"__func");

    }

}



extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_JSContext_SetPrototype__JI(JNIEnv *env, jclass type, jlong jsContext,
                                                jint idx) {

    duk_context * ctx = (duk_context *) jsContext;

    duk_set_prototype(ctx,idx);

}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_JSContext_GetGlobalString(JNIEnv *env, jclass type, jlong jsContext,
                                               jstring name_) {
    const char *name = env->GetStringUTFChars(name_, 0);

    duk_context * ctx = (duk_context *) jsContext;

    duk_get_global_string(ctx,name);

    env->ReleaseStringUTFChars(name_, name);
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_cn_kkmofang_ker_JSContext_IsObject(JNIEnv *env, jclass type, jlong jsContext, jint idx) {
    duk_context * ctx = (duk_context *) jsContext;
    return duk_is_object(ctx,idx);
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_cn_kkmofang_ker_JSContext_IsFunction(JNIEnv *env, jclass type, jlong jsContext, jint idx) {

    duk_context * ctx = (duk_context *) jsContext;
    return duk_is_function(ctx,idx);

}



extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_JSContext_GetPrototype(JNIEnv *env, jclass type, jlong jsContext, jint idx) {

    duk_context * ctx = (duk_context *) jsContext;

    duk_get_prototype(ctx,idx);

}


extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_JSContext_PutMethod(JNIEnv *env, jclass isa, jlong jsContext, jint idx,
                                         jstring name_, jstring type_, jobject method) {
    const char *name = env->GetStringUTFChars(name_, 0);
    const char *type = env->GetStringUTFChars(type_, 0);

    duk_context * ctx = (duk_context *) jsContext;

    duk_push_string(ctx,name);

    jmethodID m = env->FromReflectedMethod(method);

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
    }, JObjectMethodTypeArgumentCount(type));

    duk_push_pointer(ctx,m);
    duk_put_prop_string(ctx,-2,"__method");

    duk_push_string(ctx,type);
    duk_put_prop_string(ctx,-2,"__type");

    duk_def_prop(ctx,idx - 2,DUK_DEFPROP_CLEAR_CONFIGURABLE | DUK_DEFPROP_SET_ENUMERABLE | DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_CLEAR_WRITABLE );

    env->ReleaseStringUTFChars(name_, name);
    env->ReleaseStringUTFChars(type_, type);
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_App_dealloc(JNIEnv *env, jclass type, jlong ptr) {

    kk::ui::App * app = (kk::ui::App *) ptr;
    app->off();
    app->release();

}

extern "C"
JNIEXPORT jlong JNICALL
Java_cn_kkmofang_ker_App_jsContext(JNIEnv *env, jclass type, jlong ptr) {

    kk::ui::App * app = (kk::ui::App *) ptr;

    return (jlong) app->jsContext();

}

extern "C"
JNIEXPORT jlong JNICALL
Java_cn_kkmofang_ker_App_alloc(JNIEnv *env, jclass type, jobject object, jstring basePath_,
                                  jstring appkey_, jstring userAgent_) {
    const char *basePath = env->GetStringUTFChars(basePath_, 0);
    const char *appkey = env->GetStringUTFChars(appkey_, 0);
    const char *userAgent = env->GetStringUTFChars(userAgent_, 0);

    kk::ui::KerApp * app = new kk::ui::KerApp(env->NewWeakGlobalRef(object), basePath,userAgent,appkey);

    app->retain();

    app->on("open", new kk::TFunction<void,kk::CString,kk::Event *>([app](kk::CString name,kk::Event * event)->void{

        jboolean isAttach = false;

        JNIEnv *env = kk_env(&isAttach);

        jweak object = app->object();

        jclass isa = env->GetObjectClass(object);

        jmethodID open = env->GetMethodID(isa,"open","(Ljava/lang/String;Z)V");

        kk::TObject<kk::String, kk::Any> * data = (kk::TObject<kk::String, kk::Any> *) event->data();
        kk::CString uri = (*data)["uri"];
        kk::Boolean animated = true;

        if(data->find("animated") != data->end()) {
            animated = (*data)["animated"];
        }

        jstring URI = env->NewStringUTF(uri);

        env->CallVoidMethod(object,open,URI,(jboolean) animated);

        env->DeleteLocalRef(URI);
        env->DeleteLocalRef(isa);

        if(isAttach) {
            gJavaVm->DetachCurrentThread();
        }

    }));

    app->on("back", new kk::TFunction<void,kk::CString,kk::Event *>([app](kk::CString name,kk::Event * event)->void{

        jboolean isAttach = false;

        JNIEnv *env = kk_env(&isAttach);

        jweak object = app->object();

        jclass isa = env->GetObjectClass(object);

        jmethodID back = env->GetMethodID(isa,"back","(I;Z)V");

        kk::TObject<kk::String, kk::Any> * data = (kk::TObject<kk::String, kk::Any> *) event->data();
        kk::Uint delta = (*data)["delta"];
        kk::Boolean animated = true;

        if(data->find("animated") != data->end()) {
            animated = (*data)["animated"];
        }

        env->CallVoidMethod(object,back,(jint)delta,(jboolean) animated);

        env->DeleteLocalRef(isa);

        if(isAttach) {
            gJavaVm->DetachCurrentThread();
        }

    }));

    env->ReleaseStringUTFChars(basePath_, basePath);
    env->ReleaseStringUTFChars(appkey_, appkey);
    env->ReleaseStringUTFChars(userAgent_, userAgent);

    return (jlong) app;
}


extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_Native_setImage(JNIEnv *env, jclass type, jlong imageObject, jobject image) {

    kk::ui::OSImage * i = (kk::ui::OSImage *) imageObject;

    if(image == nullptr) {
        i->setState(kk::ui::ImageStateError);
    } else {
        i->setImage(image);
        i->setState(kk::ui::ImageStateLoaded);
    }

}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_JSContext_PushUndefined(JNIEnv *env, jclass type, jlong jsContext) {

    duk_context * ctx = (duk_context *) jsContext;

    duk_push_undefined(ctx);

}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_JSContext_PushInt(JNIEnv *env, jclass type, jlong jsContext, jint v) {

    duk_context * ctx = (duk_context *) jsContext;

    duk_push_int(ctx,v);

}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_JSContext_PushNumber(JNIEnv *env, jclass type, jlong jsContext, jdouble v) {

    duk_context * ctx = (duk_context *) jsContext;

    duk_push_number(ctx,v);

}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_JSContext_PushBoolean(JNIEnv *env, jclass type, jlong jsContext, jboolean v) {

    duk_context * ctx = (duk_context *) jsContext;

    duk_push_boolean(ctx,v);

}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_JSContext_PushString(JNIEnv *env, jclass type, jlong jsContext, jstring v_) {

    const char *v = env->GetStringUTFChars(v_, 0);

    duk_context * ctx = (duk_context *) jsContext;

    duk_push_string(ctx,v);

    env->ReleaseStringUTFChars(v_, v);
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_JSContext_PushBytes(JNIEnv *env, jclass type, jlong jsContext, jbyteArray v_) {

    jbyte *v = env->GetByteArrayElements(v_, NULL);

    jsize size = env->GetArrayLength(v_);

    duk_context * ctx = (duk_context *) jsContext;

    void * data = duk_push_fixed_buffer(ctx,size);

    memcpy(data,v,size);

    duk_push_buffer_object(ctx,-1,0,size,DUK_BUFOBJ_ARRAYBUFFER);

    duk_remove(ctx,-2);

    env->ReleaseByteArrayElements(v_, v, 0);
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_JSContext_PutProp(JNIEnv *env, jclass type, jlong jsContext, jint idx) {

    duk_context * ctx = (duk_context *) jsContext;

    duk_put_prop(ctx,idx);

}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_JSContext_PushArray(JNIEnv *env, jclass type, jlong jsContext) {

    duk_context * ctx = (duk_context *) jsContext;

    duk_push_array(ctx);

}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_JSContext_PutGlobalString(JNIEnv *env, jclass type, jlong jsContext,
                                               jstring name_) {
    const char *name = env->GetStringUTFChars(name_, 0);

    duk_context * ctx = (duk_context *) jsContext;

    duk_put_global_string(ctx,name);

    env->ReleaseStringUTFChars(name_, name);
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_JSContext_PutProperty(JNIEnv *env, jclass isa, jlong jsContext, jint idx,
                                           jstring name_, jstring type_, jobject field,
                                           jboolean readonly) {
    const char *name = env->GetStringUTFChars(name_, 0);
    const char *type = env->GetStringUTFChars(type_, 0);

    duk_context * ctx = (duk_context *) jsContext;

    duk_push_string(ctx,name);

    jfieldID fd = env->FromReflectedField(field);

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

    duk_push_pointer(ctx,fd);
    duk_put_prop_string(ctx,-2,"__field");

    duk_push_string(ctx,type);
    duk_put_prop_string(ctx,-2,"__type");

    if(readonly) {
        duk_def_prop(ctx,idx - 2,DUK_DEFPROP_CLEAR_CONFIGURABLE | DUK_DEFPROP_SET_ENUMERABLE | DUK_DEFPROP_HAVE_GETTER);
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

                if(isAttach) {
                    gJavaVm->DetachCurrentThread();
                }

                return r;
            }

            return 0;
        },0);

        duk_push_pointer(ctx,fd);
        duk_put_prop_string(ctx,-2,"__field");

        duk_push_string(ctx,type);
        duk_put_prop_string(ctx,-2,"__type");

        duk_def_prop(ctx,idx - 3,DUK_DEFPROP_CLEAR_CONFIGURABLE | DUK_DEFPROP_SET_ENUMERABLE | DUK_DEFPROP_HAVE_GETTER | DUK_DEFPROP_HAVE_SETTER);
    }

    env->ReleaseStringUTFChars(name_, name);
    env->ReleaseStringUTFChars(type_, type);
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_JSContext_PushObject__JLjava_lang_Object_2Ljava_lang_String_2(JNIEnv *env,
                                                                                   jclass type,
                                                                                   jlong jsContext,
                                                                                   jobject object,
                                                                                   jstring prototype_) {
    const char *prototype = env->GetStringUTFChars(prototype_, 0);

    duk_context * ctx = (duk_context *) jsContext;

    kk::NativeObject * native = new kk::NativeObject((kk::Native *) object);

    duk_push_object(ctx);
    kk::SetObject(ctx, -1, native);
    kk::SetPrototype(ctx,-1,prototype);

    env->ReleaseStringUTFChars(prototype_, prototype);
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_JSContext_PushObject__J(JNIEnv *env, jclass type, jlong jsContext) {

    duk_context * ctx = (duk_context *) jsContext;

    duk_push_object(ctx);

}

extern "C"
JNIEXPORT jint JNICALL
Java_cn_kkmofang_ker_JSContext_GetType(JNIEnv *env, jclass type, jlong jsContext, jint idx) {

    duk_context * ctx = (duk_context *) jsContext;

    return duk_get_type(ctx,idx);
}

extern "C"
JNIEXPORT jint JNICALL
Java_cn_kkmofang_ker_JSContext_GetTop(JNIEnv *env, jclass type, jlong jsContext) {

    duk_context * ctx = (duk_context *) jsContext;

    return duk_get_top(ctx);

}

extern "C"
JNIEXPORT jint JNICALL
Java_cn_kkmofang_ker_JSContext_ToInt(JNIEnv *env, jclass type, jlong jsContext, jint idx) {

    duk_context * ctx = (duk_context *) jsContext;

    return duk_to_int(ctx,idx);

}

extern "C"
JNIEXPORT jdouble JNICALL
Java_cn_kkmofang_ker_JSContext_ToNumber(JNIEnv *env, jclass type, jlong jsContext, jint idx) {

    duk_context * ctx = (duk_context *) jsContext;

    return duk_to_number(ctx,idx);

}

extern "C"
JNIEXPORT jboolean JNICALL
Java_cn_kkmofang_ker_JSContext_ToBoolean(JNIEnv *env, jclass type, jlong jsContext, jint idx) {

    duk_context * ctx = (duk_context *) jsContext;

    return duk_to_boolean(ctx,idx);

}

extern "C"
JNIEXPORT jobject JNICALL
Java_cn_kkmofang_ker_JSContext_ToObject(JNIEnv *env, jclass type, jlong jsContext, jint idx) {

    duk_context * ctx = (duk_context *) jsContext;

    kk::NativeObject * v = dynamic_cast<kk::NativeObject *>(kk::GetObject(ctx,idx));

    if(v != nullptr) {
        return (jobject) v->native();
    }

    return nullptr;
}

extern "C"
JNIEXPORT jbyteArray JNICALL
Java_cn_kkmofang_ker_JSContext_ToBytes(JNIEnv *env, jclass type, jlong jsContext, jint idx) {

    duk_context * ctx = (duk_context *) jsContext;

    if(duk_is_string(ctx,idx)) {
        size_t n;
        const char * p = duk_to_lstring(ctx,idx, &n);
        jbyteArray v = env->NewByteArray(n);
        env->SetByteArrayRegion(v,0,n,(jbyte *) p);
        return v;
    } else if(duk_is_buffer(ctx,idx)) {
        size_t n;
        void * p = duk_get_buffer(ctx,idx, &n);
        jbyteArray v = env->NewByteArray(n);
        env->SetByteArrayRegion(v,0,n,(jbyte *) p);
        return v;
    } else if(duk_is_buffer_data(ctx,idx)) {
        size_t n;
        void * p = duk_get_buffer_data(ctx,idx, &n);
        jbyteArray v = env->NewByteArray(n);
        env->SetByteArrayRegion(v,0,n,(jbyte *) p);
        return v;
    }

    return nullptr;
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_cn_kkmofang_ker_JSContext_IsString(JNIEnv *env, jclass type, jlong jsContext, jint idx) {

    duk_context * ctx = (duk_context *) jsContext;

    return duk_is_string(ctx,idx);

}

extern "C"
JNIEXPORT jboolean JNICALL
Java_cn_kkmofang_ker_JSContext_IsNumber(JNIEnv *env, jclass type, jlong jsContext, jint idx) {

    duk_context * ctx = (duk_context *) jsContext;

    return duk_is_number(ctx,idx);

}

extern "C"
JNIEXPORT jboolean JNICALL
Java_cn_kkmofang_ker_JSContext_IsBoolean(JNIEnv *env, jclass type, jlong jsContext, jint idx) {

    duk_context * ctx = (duk_context *) jsContext;

    return duk_is_boolean(ctx,idx);

}

extern "C"
JNIEXPORT jboolean JNICALL
Java_cn_kkmofang_ker_JSContext_IsArray(JNIEnv *env, jclass type, jlong jsContext, jint idx) {

    duk_context * ctx = (duk_context *) jsContext;

    return duk_is_array(ctx,idx);

}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_JSContext_EnumObject(JNIEnv *env, jclass type, jlong jsContext, jint idx) {

    duk_context * ctx = (duk_context *) jsContext;

    duk_enum(ctx,idx,DUK_ENUM_INCLUDE_SYMBOLS);

}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_JSContext_EnumArray(JNIEnv *env, jclass type, jlong jsContext, jint idx) {

    duk_context * ctx = (duk_context *) jsContext;

    duk_enum(ctx,idx,DUK_ENUM_ARRAY_INDICES_ONLY);

}

extern "C"
JNIEXPORT jboolean JNICALL
Java_cn_kkmofang_ker_JSContext_Next(JNIEnv *env, jclass type, jlong jsContext, jint idx,
                                    jboolean hasValue) {

    duk_context * ctx = (duk_context *) jsContext;

    return duk_next(ctx,idx,hasValue);

}extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_JSContext_Pop__J(JNIEnv *env, jclass type, jlong jsContext) {

    duk_context * ctx = (duk_context *) jsContext;

    duk_pop(ctx);

}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_JSContext_Pop__JI(JNIEnv *env, jclass type, jlong jsContext, jint n) {

    duk_context * ctx = (duk_context *) jsContext;

    duk_pop_n(ctx,n);

}

extern "C"
JNIEXPORT jobject JNICALL
Java_cn_kkmofang_ker_JSContext_ToJSObject(JNIEnv *env, jclass type, jlong jsContext, jint idx) {

    duk_context * ctx = (duk_context *) jsContext;

    if(duk_is_object(ctx,idx) || duk_is_function(ctx,idx)) {

        kk::JSObject * v = new kk::JSObject(ctx,duk_get_heapptr(ctx,idx));

        v->retain();

        jclass isa =env->FindClass("cn/kkmofang/ker/Native");

        jmethodID allocJSObject = env->GetMethodID(isa,"allocJSObject","(J)Lcn/kkmofang/ker/JSObject;");

        jobject native = env->CallStaticObjectMethod(isa,allocJSObject,(jlong) v);

        env->DeleteLocalRef(isa);

        v->release();

        return native;

    }

    return nullptr;
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_Native_loop(JNIEnv *env, jclass type) {

    event_base * base = kk::GetDispatchQueueEventBase(kk::mainDispatchQueue());
    event_base_loop(base,EVLOOP_NONBLOCK);

}

extern "C"
JNIEXPORT jlong JNICALL
Java_cn_kkmofang_ker_App_run__JLjava_lang_Object_2(JNIEnv *env, jclass type, jlong ptr,
                                                   jobject query) {

    kk::ui::App * app = (kk::ui::App *) ptr;

    kk::NativeValue * v = nullptr;

    if(query != nullptr) {
        v = new kk::NativeValue((kk::Native *) query);
    }

    app->exec("main.js", new kk::TObject<kk::String, kk::Any>({{"query",v}}));

}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_App_openlib__(JNIEnv *env, jclass type) {

    kk::ui::App::Openlib();

}