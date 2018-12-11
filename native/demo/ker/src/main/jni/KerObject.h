//
// Created by zhanghailong on 2016/11/7.
//

#ifndef KerObject_H
#define KerObject_H

#include <jni.h>
#include <duktape.h>

#ifdef __cplusplus
extern "C" {
#endif

    void duk_push_JObject(duk_context * ctx, jobject object);

    jobject duk_to_JObject(JNIEnv * env, duk_context * ctx, duk_idx_t idx);

#ifdef __cplusplus
}
#endif

#endif //KerObject_H
