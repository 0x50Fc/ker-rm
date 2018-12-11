#include <jni.h>

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_Native_retain(JNIEnv *env, jclass type, jlong kerObject) {

    // TODO

}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_Native_release(JNIEnv *env, jclass type, jlong kerObject) {

    // TODO

}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_JSContext_PushFunction(JNIEnv *env, jclass type, jlong jsContext,
                                            jobject func) {

    // TODO

}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_JSContext_PushObject(JNIEnv *env, jclass type, jlong jsContext) {

    // TODO

}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_JSContext_SetPrototype__JI(JNIEnv *env, jclass type, jlong jsContext,
                                                jint idx) {

    // TODO

}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_JSContext_PushGlobalString(JNIEnv *env, jclass type, jlong jsContext,
                                                jstring name_) {
    const char *name = env->GetStringUTFChars(name_, 0);

    // TODO

    env->ReleaseStringUTFChars(name_, name);
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_JSContext_GetGlobalString(JNIEnv *env, jclass type, jlong jsContext,
                                               jstring name_) {
    const char *name = env->GetStringUTFChars(name_, 0);

    // TODO

    env->ReleaseStringUTFChars(name_, name);
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_cn_kkmofang_ker_JSContext_IsObject(JNIEnv *env, jclass type, jlong jsContext, jint idx) {

    // TODO

}

extern "C"
JNIEXPORT jboolean JNICALL
Java_cn_kkmofang_ker_JSContext_IsFunction(JNIEnv *env, jclass type, jlong jsContext, jint idx) {

    // TODO

}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_JSContext_Pop(JNIEnv *env, jclass type, jlong jsContext) {

    // TODO

}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_JSContext_GetPrototype(JNIEnv *env, jclass type, jlong jsContext, jint idx) {

    // TODO

}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_JSContext_PutProperty(JNIEnv *env, jclass type, jlong jsContext, jint idx,
                                           jstring name_, jobject field, jboolean readonly) {
    const char *name = env->GetStringUTFChars(name_, 0);

    // TODO

    env->ReleaseStringUTFChars(name_, name);
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_ker_JSContext_PutMethod(JNIEnv *env, jclass isa, jlong jsContext, jint idx,
                                         jstring name_, jstring type_, jobject method) {
    const char *name = env->GetStringUTFChars(name_, 0);
    const char *type = env->GetStringUTFChars(type_, 0);

    // TODO

    env->ReleaseStringUTFChars(name_, name);
    env->ReleaseStringUTFChars(type_, type);
}

