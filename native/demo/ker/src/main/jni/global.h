//
// Created by zhanghailong on 2018/12/27.
//

#ifndef KER_GLOBAL_H
#define KER_GLOBAL_H

#include <jni.h>


#ifdef __cplusplus
extern "C" {
#endif

struct _Global {
    struct {
        jclass isa;
        jmethodID gc;
        jmethodID allocJSObject;
        jmethodID getImageWidth;
        jmethodID getImageHeight;
        jmethodID getImage;
        jmethodID pushObject;
        jmethodID toObject;
        jmethodID getPrototype;
        jmethodID runApp;
        jmethodID getPackage;
        jmethodID appendText;
        jmethodID appendImage;
        jmethodID viewObtain;
        jmethodID viewRecycle;
        jmethodID viewSetAttribute;
        jmethodID viewSetFrame;
        jmethodID viewSetContentSize;
        jmethodID viewSetContentOffset;
        jmethodID viewGetContentOffsetX;
        jmethodID viewGetContentOffsetY;
        jmethodID viewAddSubview;
        jmethodID viewRemoveView;
        jmethodID viewEvaluateJavaScript;
        jmethodID viewSetAttributedText;
        jmethodID viewSetImage;
        jmethodID viewSetGravity;
        jmethodID viewSetContent;
        jmethodID createView;
        jmethodID getAttributedTextSize;
    } Native;
    struct {
        jclass isa;
        jmethodID invoke;
    } JSFunction;
    struct  {
        jclass isa;
        jmethodID open;
        jmethodID back;
    } App;
    struct {
        jclass isa;
        jmethodID setOptions;
        jmethodID close;
    } Page;
    struct  {
        jclass isa;
        jmethodID init;
        jfieldID userScripts;
        jfieldID userActions;
        struct {
            jclass isa;
            jmethodID init;
            jfieldID source;
            jfieldID type;
        } UserScript;
        struct {
            jclass isa;
            jmethodID init;
            jfieldID pattern;
            jfieldID policy;
        } UserAction;
    } WebViewConfiguration;
    struct {
        jclass isa;
        jmethodID run;
    } Runnable;

    struct {
        jclass isa;
        jmethodID init;
    } Integer;

    struct {
        jclass isa;
        jmethodID init;
    } Boolean;


    struct {
        jclass isa;
        jmethodID init;
    } Long;


    struct {
        jclass isa;
        jmethodID init;
    } Float;


    struct {
        jclass isa;
        jmethodID init;
    } Double;

    struct {
        jclass isa;
        jmethodID init;
        jmethodID put;
    } TreeMap;

    struct {
        jclass isa;
        jmethodID init;
        jmethodID add;
    } ArrayList;

    struct {
        jclass isa;
        jmethodID init;
    } SpannableStringBuilder;

};

extern struct _Global G;


void globalInit(JNIEnv  * env);

#ifdef __cplusplus
}
#endif


#endif //DEMO_GLOBAL_H
