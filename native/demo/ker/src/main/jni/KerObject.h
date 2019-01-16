//
// Created by zhanghailong on 2016/11/7.
//

#ifndef KerObject_H
#define KerObject_H

#include <jni.h>
#include <core/jit.h>

#ifdef __cplusplus
extern "C" {
#endif

    void ker_JObject_to_Any(JNIEnv * env,jobject object, kk::Any & returnValue);

    jobject ker_Object_to_JObject(JNIEnv * env, kk::Object * object);

    jobject ker_to_JObject(JNIEnv * env, kk::Any &v);

    void duk_push_JObject(duk_context * ctx, jobject object);

    jobject duk_to_JObject(JNIEnv * env, duk_context * ctx, duk_idx_t idx);

    jboolean duk_to_JBoolean(JNIEnv * env, duk_context * ctx, duk_idx_t idx);

    jbyte duk_to_JByte(JNIEnv * env, duk_context * ctx, duk_idx_t idx);

    jchar duk_to_JChar(JNIEnv * env, duk_context * ctx, duk_idx_t idx);

    jshort duk_to_JShort(JNIEnv * env, duk_context * ctx, duk_idx_t idx);

    jint duk_to_JInt(JNIEnv * env, duk_context * ctx, duk_idx_t idx);

    jlong duk_to_JLong(JNIEnv * env, duk_context * ctx, duk_idx_t idx);

    jfloat duk_to_JFloat(JNIEnv * env, duk_context * ctx, duk_idx_t idx);

    jdouble duk_to_JDouble(JNIEnv * env, duk_context * ctx, duk_idx_t idx);

    duk_ret_t JObjectGetProperty(JNIEnv * env,jobject object,jfieldID field,kk::CString type,duk_context * ctx);

    duk_ret_t JObjectSetProperty(JNIEnv * env,jobject object,jfieldID field,kk::CString type,duk_context * ctx);

    duk_ret_t JObjectInvoke(JNIEnv * env,jobject object,jmethodID method,kk::CString types,duk_context * ctx,duk_idx_t top);

    duk_int_t JObjectMethodTypeArgumentCount(kk::CString types);

#ifdef __cplusplus
}
#endif


#endif //KerObject_H
