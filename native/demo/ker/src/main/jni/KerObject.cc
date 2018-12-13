//
// Created by zhanghailong on 2016/11/7.
//

#include "kk.h"
#include "KerObject.h"
#include <core/kk.h>
#include <core/jit.h>

jboolean duk_to_JBoolean(JNIEnv * env, duk_context * ctx, duk_idx_t idx) {

    if(idx >= 0) {
        return false;
    }

    switch (duk_get_type(ctx,idx)) {
        case DUK_TYPE_BOOLEAN:
            return duk_to_boolean(ctx,idx);
        case DUK_TYPE_NUMBER:
            return duk_to_number(ctx,idx) != 0;
        case DUK_TYPE_STRING:
        {
            kk::CString v = duk_to_string(ctx,idx);
            return !kk::CStringEqual(v,"");
        }
        case DUK_TYPE_OBJECT:
        case DUK_TYPE_BUFFER:
        case DUK_TYPE_LIGHTFUNC:
            return true;

    }

    return false;
}

jbyte duk_to_JByte(JNIEnv * env, duk_context * ctx, duk_idx_t idx) {

    if(idx >= 0) {
        return 0;
    }

    switch (duk_get_type(ctx,idx)) {
        case DUK_TYPE_BOOLEAN:
            return duk_to_boolean(ctx,idx);
        case DUK_TYPE_NUMBER:
            return duk_to_int(ctx,idx);
        case DUK_TYPE_STRING:
        {
            kk::CString v = duk_to_string(ctx,idx);
            return atoi(v);
        }
    }

    return 0;

}

jchar duk_to_JChar(JNIEnv * env, duk_context * ctx, duk_idx_t idx) {

    if(idx >= 0) {
        return 0;
    }

    switch (duk_get_type(ctx,idx)) {
        case DUK_TYPE_BOOLEAN:
            return duk_to_boolean(ctx,idx);
        case DUK_TYPE_NUMBER:
            return duk_to_int(ctx,idx);
        case DUK_TYPE_STRING:
        {
            kk::CString v = duk_to_string(ctx,idx);
            return atoi(v);
        }
    }

    return 0;
}

jshort duk_to_JShort(JNIEnv * env, duk_context * ctx, duk_idx_t idx) {

    if(idx >= 0) {
        return 0;
    }

    switch (duk_get_type(ctx,idx)) {
        case DUK_TYPE_BOOLEAN:
            return duk_to_boolean(ctx,idx);
        case DUK_TYPE_NUMBER:
            return duk_to_int(ctx,idx);
        case DUK_TYPE_STRING:
        {
            kk::CString v = duk_to_string(ctx,idx);
            return atoi(v);
        }
    }

    return 0;
}

jint duk_to_JInt(JNIEnv * env, duk_context * ctx, duk_idx_t idx) {
    if(idx >= 0) {
        return 0;
    }

    switch (duk_get_type(ctx,idx)) {
        case DUK_TYPE_BOOLEAN:
            return duk_to_boolean(ctx,idx);
        case DUK_TYPE_NUMBER:
            return duk_to_int(ctx,idx);
        case DUK_TYPE_STRING:
        {
            kk::CString v = duk_to_string(ctx,idx);
            return atoi(v);
        }
    }

    return 0;
}

jlong duk_to_JLong(JNIEnv * env, duk_context * ctx, duk_idx_t idx) {

    if(idx >= 0) {
        return 0;
    }

    switch (duk_get_type(ctx,idx)) {
        case DUK_TYPE_BOOLEAN:
            return duk_to_boolean(ctx,idx);
        case DUK_TYPE_NUMBER:
            return duk_to_int(ctx,idx);
        case DUK_TYPE_STRING:
        {
            kk::CString v = duk_to_string(ctx,idx);
            return atoll(v);
        }
    }

    return 0;
}

jfloat duk_to_JFloat(JNIEnv * env, duk_context * ctx, duk_idx_t idx) {
    if(idx >= 0) {
        return 0;
    }

    switch (duk_get_type(ctx,idx)) {
        case DUK_TYPE_BOOLEAN:
            return duk_to_boolean(ctx,idx);
        case DUK_TYPE_NUMBER:
            return duk_to_number(ctx,idx);
        case DUK_TYPE_STRING:
        {
            kk::CString v = duk_to_string(ctx,idx);
            return atof(v);
        }
    }

    return 0;
}

jdouble duk_to_JDouble(JNIEnv * env, duk_context * ctx, duk_idx_t idx) {
    if(idx >= 0) {
        return 0;
    }

    switch (duk_get_type(ctx,idx)) {
        case DUK_TYPE_BOOLEAN:
            return duk_to_boolean(ctx,idx);
        case DUK_TYPE_NUMBER:
            return duk_to_number(ctx,idx);
        case DUK_TYPE_STRING:
        {
            kk::CString v = duk_to_string(ctx,idx);
            return atof(v);
        }
    }

    return 0;
}

duk_ret_t JObjectGetProperty(JNIEnv * env,jobject object,jfieldID field,kk::CString type,duk_context * ctx) {

    switch (* type) {
        case 'Z':
        {
            duk_push_boolean(ctx,env->GetBooleanField(object,field));
            return 1;
        }
        case 'B':
        {
            duk_push_int(ctx,env->GetByteField(object,field));
            return 1;
        }
        case 'C':
        {
            duk_push_int(ctx,env->GetCharField(object,field));
            return 1;
        }
        case 'S':
        {
            duk_push_int(ctx,env->GetShortField(object,field));
            return 1;
        }
        case 'I':
        {
            duk_push_int(ctx,env->GetIntField(object,field));
            return 1;
        }
        case 'J':
        {
            duk_push_int(ctx,env->GetLongField(object,field));
            return 1;
        }
        case 'F':
        {
            duk_push_number(ctx,env->GetFloatField(object,field));
            return 1;
        }
        case 'D':
        {
            duk_push_number(ctx,env->GetDoubleField(object,field));
            return 1;
        }
        case 'L':
        {
            jobject v = env->GetObjectField(object,field);
            if(v != nullptr) {
                duk_push_JObject(ctx,v);
                return 1;
            }
        }

    }

    return 0;
}

duk_ret_t JObjectSetProperty(JNIEnv * env,jobject object,jfieldID field,kk::CString type,duk_context * ctx) {

    switch (* type) {
        case 'Z':
        {
            env->SetBooleanField(object,field,duk_to_JBoolean(env,ctx,-1));
            return 1;
        }
        case 'B':
        {
            env->SetByteField(object,field,duk_to_JByte(env,ctx,-1));
            return 1;
        }
        case 'C':
        {
            env->SetCharField(object,field,duk_to_JChar(env,ctx,-1));
            return 1;
        }
        case 'S':
        {
            env->SetShortField(object,field,duk_to_JShort(env,ctx,-1));
            return 1;
        }
        case 'I':
        {
            env->SetIntField(object,field,duk_to_JInt(env,ctx,-1));
            return 1;
        }
        case 'J':
        {
            env->SetLongField(object,field,duk_to_JLong(env,ctx,-1));
            return 1;
        }
        case 'F':
        {
            env->SetFloatField(object,field,duk_to_JFloat(env,ctx,-1));
            return 1;
        }
        case 'D':
        {
            env->SetDoubleField(object,field,duk_to_JDouble(env,ctx,-1));
            return 1;
        }
        case 'L':
        {
            env->SetObjectField(object,field,duk_to_JObject(env,ctx,-1));
        }

    }

    return 0;

}

duk_int_t JObjectMethodTypeArgumentCount(kk::CString types) {

    char * p = (char *) types;
    int s = 0;
    int count = 0;

    while(p && *p) {

        if(s == 0) {
            if(*p == '(') {
                s = 1;
            }
        } else if(s == 1) {
            if(*p == ')') {
                s = 2;
            } else {
                count ++;
                if(*p == 'L') {
                    while(p && *p) {
                        if(*p == ';') {
                            break;
                        }
                        p ++;
                    }
                }

            }
        } else {
            break;
        }

        p ++;

    }

    return count;

}

duk_ret_t JObjectInvoke(JNIEnv * env,jobject object,jmethodID method,kk::CString types,duk_context * ctx,duk_idx_t top) {

    std::vector<jvalue> vs;
    std::vector<jobject> objects;

    jvalue v;
    char * p = (char *) types;
    int s = 0;

    while(p && *p) {

        if(s == 0) {
            if(*p == '(') {
                s = 1;
            }
        } else if(s == 1) {
            if(*p == ')') {
                s = 2;
            } else {
                switch (* p) {
                    case 'Z':
                        v.z = duk_to_JBoolean(env,ctx,-top);
                        vs.push_back(v);
                        top ++;
                        break;
                    case 'B':
                        v.b = duk_to_JByte(env,ctx,-top);
                        vs.push_back(v);
                        top ++;
                        break;
                    case 'C':
                        v.c = duk_to_JChar(env,ctx,-top);
                        vs.push_back(v);
                        top ++;
                        break;
                    case 'S':
                        v.s = duk_to_JShort(env,ctx,-top);
                        vs.push_back(v);
                        top ++;
                        break;
                    case 'I':
                        v.i = duk_to_JInt(env,ctx,-top);
                        vs.push_back(v);
                        top ++;
                        break;
                    case 'J':
                        v.j = duk_to_JLong(env,ctx,-top);
                        vs.push_back(v);
                        top ++;
                        break;
                    case 'F':
                        v.f = duk_to_JFloat(env,ctx,-top);
                        vs.push_back(v);
                        top ++;
                        break;
                    case 'D':
                        v.d = duk_to_JDouble(env,ctx,-top);
                        vs.push_back(v);
                        top ++;
                        break;
                    case 'L':
                        v.l = duk_to_JObject(env,ctx,-top);
                        vs.push_back(v);
                        objects.push_back(v.l);
                        top ++;
                        while(p && *p) {
                            if(*p == ';') {
                                break;
                            }
                            p ++;
                        }
                        break;
                    default:
                        assert(0);
                        break;

                }
            }
        } else {

            switch (* p) {
                case 'Z':
                    duk_push_boolean(ctx,env->CallBooleanMethodA(object,method,vs.data()));
                    return 1;
                case 'B':
                    duk_push_int(ctx,env->CallByteMethodA(object,method,vs.data()));
                    return 1;
                case 'C':
                    duk_push_int(ctx,env->CallCharMethodA(object,method,vs.data()));
                    return 1;
                case 'S':
                    duk_push_int(ctx,env->CallShortMethodA(object,method,vs.data()));
                    return 1;
                case 'I':
                    duk_push_int(ctx,env->CallIntMethodA(object,method,vs.data()));
                    return 1;
                case 'J':
                    duk_push_int(ctx,env->CallLongMethodA(object,method,vs.data()));
                    return 1;
                case 'F':
                    duk_push_number(ctx,env->CallFloatMethodA(object,method,vs.data()));
                    return 1;
                case 'D':
                    duk_push_number(ctx,env->CallDoubleMethodA(object,method,vs.data()));
                    return 1;
                case 'L':
                {
                    jobject v = env->CallObjectMethodA(object,method,vs.data());
                    if(v == nullptr){
                        return 0;
                    } else {
                        duk_push_JObject(ctx,v);
                        env->DeleteLocalRef(v);
                        return 1;
                    }
                }
                case 'V':
                    env->CallVoidMethodA(object,method,vs.data());
                    break;
                default:
                    assert(0);
                    break;

            }

            break;
        }

        p ++;

    }

    return 0;

}

void duk_push_JObject(duk_context * ctx, jobject object) {

    jboolean isAttach = false;

    JNIEnv *env = kk_env(&isAttach);

    jclass isa = env->FindClass("cn/kkmofang/ker/Native");

    jmethodID pushObject = env->GetStaticMethodID(isa,"pushObject","(JLjava/lang/Object;)V");

    env->CallStaticVoidMethod(isa,pushObject,(jlong)ctx, object);

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

            env->DeleteGlobalRef((jobject) _native);

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
