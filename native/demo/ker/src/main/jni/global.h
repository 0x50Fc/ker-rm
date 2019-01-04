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
        const char * userAgent;
        jmethodID getImage;
        jmethodID copyPixels;
        jmethodID getDirectory;
        jmethodID createPage;
        jmethodID openlib;
        jmethodID appendText;
        jmethodID appendImage;
        jmethodID getAttributedTextSize;
        jmethodID displayCanvas;
        jmethodID allocJSObject;
        jmethodID getPrototype;
        jmethodID openPageViewController;
        jmethodID popPageViewController;
        jmethodID pageSetOptions;
        jmethodID pageClose;
        jmethodID createView;
        jmethodID createNativeView;
        jmethodID deleteView;
        jmethodID setAttribute;
        jmethodID setFrame;
        jmethodID addSubview;
        jmethodID removeView;
        jmethodID setContentSize;
        jmethodID setContentOffset;
        jmethodID setContent;
        jmethodID setImage;
        jmethodID evaluateJavaScript;
        jmethodID setAttributedText;
        struct {
            jclass isa;
            jmethodID onOpen;
            jmethodID onError;
        } OpenCallback;
    } UI;

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
        jmethodID doubleValue;
    } Number;

    struct {
        jclass isa;
        jmethodID init;
        jmethodID intValue;
    } Integer;

    struct {
        jclass isa;
        jmethodID init;
        jmethodID booleanValue;
    } Boolean;


    struct {
        jclass isa;
        jmethodID init;
        jmethodID longValue;
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
        jmethodID hasNext;
        jmethodID next;
    } Iterator;

    struct {
        jclass isa;
        jmethodID iterator;
    } Collection;

    struct {
        jclass isa;
        jmethodID entrySet;
        struct {
            jclass isa;
            jmethodID getKey;
            jmethodID getValue;
        } Entry;
    } Map;

    struct {
        jclass isa;
        jmethodID init;
        jmethodID put;
    } TreeMap;

    struct {
        jclass isa;
        jmethodID getLength;
        jmethodID get;
    } Array;

    struct {
        jclass isa;
        jmethodID init;
        jmethodID add;
    } ArrayList;

    struct {
        jclass isa;
        jmethodID init;
    } SpannableStringBuilder;

    struct {
        jclass isa;
        jmethodID init;
    } Canvas;

    struct {
        jclass isa;
        jmethodID isArray;
        jmethodID getFields;
        jmethodID getName;
    } Class;

    struct {
        jclass isa;
        jmethodID getName;
        jmethodID get;
    } Field;

    struct {
        jclass isa;
    } String;

    struct {
        jclass isa;
        jmethodID ptr;
    } JSObject;

    struct {
        jclass isa;
        jfieldID string;
    } JSONString;

    struct {
        jclass isa;
        jmethodID send;
    } Http;

    struct {
        jclass isa;
        jmethodID cancel;
    } SessionTask;

};

extern struct _Global G;


void globalInit(JNIEnv  * env);

#ifdef __cplusplus
}
#endif


#endif //DEMO_GLOBAL_H
