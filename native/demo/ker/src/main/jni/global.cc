//
// Created by zhanghailong on 2018/12/27.
//

#include "global.h"
#include <string.h>
#include <malloc.h>

struct _Global G = {0};

void globalInit(JNIEnv  * env) {


    {
        jclass isa = env->FindClass("cn/kkmofang/ker/KerUI");
        G.UI.isa = (jclass) env->NewGlobalRef(isa);
        G.UI.getImage = env->GetStaticMethodID(isa,"getImage","(JLjava/lang/String;)V");
        G.UI.copyPixels = env->GetStaticMethodID(isa,"copyPixels","(Ljava/lang/Object;)[B");
        G.UI.getDirectory = env->GetStaticMethodID(isa,"getDirectory","(Ljava/lang/String;)Ljava/lang/String;");
        G.UI.createPage = env->GetStaticMethodID(isa,"createPage","(JJ)V");
        G.UI.openlib = env->GetStaticMethodID(isa,"openlib","(J)V");
        G.UI.appendText = env->GetStaticMethodID(isa,"appendText","(Landroid/text/SpannableStringBuilder;Ljava/lang/String;Ljava/lang/String;FZZI)V");
        G.UI.appendImage = env->GetStaticMethodID(isa,"appendImage","(Landroid/text/SpannableStringBuilder;Ljava/lang/Object;IIIIII)V");
        G.UI.getAttributedTextSize = env->GetStaticMethodID(isa,"getAttributedTextSize","(Ljava/lang/CharSequence;F)[F");
        G.UI.displayCanvas = env->GetStaticMethodID(isa,"displayCanvas","(Landroid/graphics/Bitmap;JJ)V");
        G.UI.createBitmap = env->GetStaticMethodID(isa,"createBitmap","(II)Landroid/graphics/Bitmap;");
        G.UI.allocJSObject = env->GetStaticMethodID(isa,"allocJSObject","(J)Lcn/kkmofang/ker/JSObject;");
        G.UI.getPrototype = env->GetStaticMethodID(isa,"getPrototype","(Ljava/lang/Class;)Ljava/lang/String;");
        G.UI.openPageViewController = env->GetStaticMethodID(isa,"openPageViewController","(JZLjava/lang/String;)V");
        G.UI.popPageViewController = env->GetStaticMethodID(isa,"popPageViewController","(IZ)V");
        G.UI.pageSetOptions = env->GetStaticMethodID(isa,"pageSetOptions","(JLjava/lang/Object;)V");
        G.UI.pageClose = env->GetStaticMethodID(isa,"pageClose","(JZ)V");
        G.UI.createView = env->GetStaticMethodID(isa,"createView","(Ljava/lang/String;Lcn/kkmofang/ker/WebViewConfiguration;JJ)V");
        G.UI.createNativeView = env->GetStaticMethodID(isa,"createView","(Landroid/view/View;JJ)V");
        G.UI.deleteView = env->GetStaticMethodID(isa,"deleteView","(JJ)V");
        G.UI.setAttribute = env->GetStaticMethodID(isa,"setAttribute","(JLjava/lang/String;Ljava/lang/String;J)V");
        G.UI.setFrame = env->GetStaticMethodID(isa,"setFrame","(JIIIIJ)V");
        G.UI.addSubview = env->GetStaticMethodID(isa,"addSubview","(JJIJ)V");
        G.UI.removeView = env->GetStaticMethodID(isa,"removeView","(JJ)V");
        G.UI.setContentSize = env->GetStaticMethodID(isa,"setContentSize","(JIIJ)V");
        G.UI.setContentOffset = env->GetStaticMethodID(isa,"setContentOffset","(JIIZJ)V");
        G.UI.setContent = env->GetStaticMethodID(isa,"setContent","(JLjava/lang/String;Ljava/lang/String;Ljava/lang/String;J)V");
        G.UI.setImage = env->GetStaticMethodID(isa,"setImage","(JLjava/lang/Object;J)V");
        G.UI.evaluateJavaScript = env->GetStaticMethodID(isa,"evaluateJavaScript","(JLjava/lang/String;J)V");
        G.UI.setAttributedText = env->GetStaticMethodID(isa,"setAttributedText","(JLjava/lang/CharSequence;J)V");
        G.UI.commit = env->GetStaticMethodID(isa,"commit","()V");

        {
            jmethodID v = env->GetStaticMethodID(isa,"userAgent","()Ljava/lang/String;");
            jstring s = (jstring) env->CallStaticObjectMethod(isa,v);
            const char * c = env->GetStringUTFChars(s, nullptr);
            G.UI.userAgent = (const char *) malloc(strlen(c) + 1);
            strcpy((char *) G.UI.userAgent,c);
            env->ReleaseStringUTFChars(s,c);
            env->DeleteLocalRef(s);
        }
        env->DeleteLocalRef(isa);

        {
            jclass isa = env->FindClass("cn/kkmofang/ker/KerUI$OpenCallback");
            G.UI.OpenCallback.isa = (jclass) env->NewGlobalRef(isa);
            G.UI.OpenCallback.onOpen = env->GetMethodID(isa,"onOpen","(J)V");
            G.UI.OpenCallback.onError = env->GetMethodID(isa,"onError","(Ljava/lang/String;)V");
            env->DeleteLocalRef(isa);
        }
    }


    {
        jclass isa = env->FindClass("cn/kkmofang/ker/WebViewConfiguration");
        G.WebViewConfiguration.isa = (jclass) env->NewGlobalRef(isa);
        G.WebViewConfiguration.init = env->GetMethodID(isa,"<init>","()V");
        G.WebViewConfiguration.userScripts = env->GetFieldID(isa,"userScripts","[Lcn/kkmofang/ker/WebViewConfiguration$UserScript;");
        G.WebViewConfiguration.userActions = env->GetFieldID(isa,"userActions","[Lcn/kkmofang/ker/WebViewConfiguration$UserAction;");
        env->DeleteLocalRef(isa);

        {
            jclass isa = env->FindClass("cn/kkmofang/ker/WebViewConfiguration$UserScript");
            G.WebViewConfiguration.UserScript.isa = (jclass) env->NewGlobalRef(isa);
            G.WebViewConfiguration.UserScript.init = env->GetMethodID(isa,"<init>","()V");
            G.WebViewConfiguration.UserScript.source = env->GetFieldID(isa,"source","Ljava/lang/String;");
            G.WebViewConfiguration.UserScript.type = env->GetFieldID(isa,"type","I");
            env->DeleteLocalRef(isa);
        }

        {
            jclass isa = env->FindClass("cn/kkmofang/ker/WebViewConfiguration$UserAction");
            G.WebViewConfiguration.UserAction.isa = (jclass) env->NewGlobalRef(isa);
            G.WebViewConfiguration.UserAction.init = env->GetMethodID(isa,"<init>","()V");
            G.WebViewConfiguration.UserAction.pattern = env->GetFieldID(isa,"pattern","Ljava/lang/String;");
            G.WebViewConfiguration.UserAction.policy = env->GetFieldID(isa,"policy","I");
            env->DeleteLocalRef(isa);
        }
    }

    {
        jclass isa = env->FindClass("android/graphics/Bitmap");
        G.Bitmap.isa = (jclass) env->NewGlobalRef(isa);
        G.Bitmap.recycle = env->GetMethodID(isa,"recycle","()V");
        env->DeleteLocalRef(isa);
    }

    {
        jclass isa = env->FindClass("java/lang/Object");
        G.Object.isa = (jclass) env->NewGlobalRef(isa);
        env->DeleteLocalRef(isa);
    }

    {
        jclass isa = env->FindClass("java/lang/Number");
        G.Number.isa = (jclass) env->NewGlobalRef(isa);
        G.Number.doubleValue = env->GetMethodID(isa,"doubleValue","()D");
        G.Number.intValue = env->GetMethodID(isa,"intValue","()I");
        G.Number.longValue = env->GetMethodID(isa,"longValue","()J");
        env->DeleteLocalRef(isa);
    }

    {
        jclass isa = env->FindClass("java/lang/Integer");
        G.Integer.isa = (jclass) env->NewGlobalRef(isa);
        G.Integer.init = env->GetMethodID(isa,"<init>","(I)V");
        G.Integer.intValue = env->GetMethodID(isa,"intValue","()I");
        env->DeleteLocalRef(isa);
    }

    {
        jclass isa = env->FindClass("java/lang/Long");
        G.Long.isa = (jclass) env->NewGlobalRef(isa);
        G.Long.init = env->GetMethodID(isa,"<init>","(J)V");
        G.Long.longValue = env->GetMethodID(isa,"longValue","()J");
        env->DeleteLocalRef(isa);
    }

    {
        jclass isa = env->FindClass("java/lang/Boolean");
        G.Boolean.isa = (jclass) env->NewGlobalRef(isa);
        G.Boolean.init = env->GetMethodID(isa,"<init>","(Z)V");
        G.Boolean.booleanValue = env->GetMethodID(isa,"booleanValue","()Z");
        env->DeleteLocalRef(isa);
    }

    {
        jclass isa = env->FindClass("java/lang/Float");
        G.Float.isa = (jclass) env->NewGlobalRef(isa);
        G.Float.init = env->GetMethodID(isa,"<init>","(F)V");
        env->DeleteLocalRef(isa);
    }

    {
        jclass isa = env->FindClass("java/lang/Double");
        G.Double.isa = (jclass) env->NewGlobalRef(isa);
        G.Double.init = env->GetMethodID(isa,"<init>","(D)V");
        env->DeleteLocalRef(isa);
    }

    {
        jclass isa = env->FindClass("java/util/Map");
        G.Map.isa = (jclass) env->NewGlobalRef(isa);
        G.Map.entrySet = env->GetMethodID(isa,"entrySet","()Ljava/util/Set;");
        G.Map.containsKey = env->GetMethodID(isa,"containsKey","(Ljava/lang/Object;)Z");
        G.Map.get = env->GetMethodID(isa,"get","(Ljava/lang/Object;)Ljava/lang/Object;");
        {
            jclass isa = env->FindClass("java/util/Map$Entry");
            G.Map.Entry.isa = (jclass) env->NewGlobalRef(isa);
            G.Map.Entry.getKey = env->GetMethodID(isa,"getKey","()Ljava/lang/Object;");
            G.Map.Entry.getValue = env->GetMethodID(isa,"getValue","()Ljava/lang/Object;");
            env->DeleteLocalRef(isa);
        }
        env->DeleteLocalRef(isa);
    }

    {
        jclass isa = env->FindClass("java/util/Collection");
        G.Collection.isa = (jclass) env->NewGlobalRef(isa);
        G.Collection.iterator = env->GetMethodID(isa,"iterator","()Ljava/util/Iterator;");
        env->DeleteLocalRef(isa);
    }

    {
        jclass isa = env->FindClass("java/util/Iterator");
        G.Iterator.isa = (jclass) env->NewGlobalRef(isa);
        G.Iterator.hasNext = env->GetMethodID(isa,"hasNext","()Z");
        G.Iterator.next = env->GetMethodID(isa,"next","()Ljava/lang/Object;");
        env->DeleteLocalRef(isa);
    }

    {
        jclass isa = env->FindClass("java/util/TreeMap");
        G.TreeMap.isa = (jclass) env->NewGlobalRef(isa);
        G.TreeMap.init = env->GetMethodID(isa,"<init>","()V");
        G.TreeMap.put = env->GetMethodID(isa,"put","(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
        env->DeleteLocalRef(isa);
    }

    {
        jclass isa = env->FindClass("java/lang/reflect/Array");
        G.Array.isa = (jclass) env->NewGlobalRef(isa);
        G.Array.getLength = env->GetStaticMethodID(isa,"getLength","(Ljava/lang/Object;)I");
        G.Array.get =  env->GetStaticMethodID(isa,"get","(Ljava/lang/Object;I)Ljava/lang/Object;");
        env->DeleteLocalRef(isa);
    }

    {
        jclass isa = env->FindClass("java/util/ArrayList");
        G.ArrayList.isa = (jclass) env->NewGlobalRef(isa);
        G.ArrayList.init = env->GetMethodID(isa,"<init>","()V");
        G.ArrayList.add =  env->GetMethodID(isa,"add","(Ljava/lang/Object;)Z");
        env->DeleteLocalRef(isa);
    }

    {
        jclass isa = env->FindClass("android/text/SpannableStringBuilder");
        G.SpannableStringBuilder.isa = (jclass) env->NewGlobalRef(isa);
        G.SpannableStringBuilder.init = env->GetMethodID(isa,"<init>","()V");
        env->DeleteLocalRef(isa);
    }

    {
        jclass isa = env->FindClass("java/lang/reflect/Field");
        G.Field.isa = (jclass) env->NewGlobalRef(isa);
        G.Field.getName = env->GetMethodID(isa,"getName","()Ljava/lang/String;");
        G.Field.get = env->GetMethodID(isa,"get","(Ljava/lang/Object;)Ljava/lang/Object;");
        env->DeleteLocalRef(isa);
    }

    {
        jclass isa = env->FindClass("java/lang/Class");
        G.Class.isa = (jclass) env->NewGlobalRef(isa);
        G.Class.isArray = env->GetMethodID(isa,"isArray","()Z");
        G.Class.getFields = env->GetMethodID(isa,"getFields","()[Ljava/lang/reflect/Field;");
        G.Class.getName = env->GetMethodID(isa,"getName","()Ljava/lang/String;");
        env->DeleteLocalRef(isa);
    }

    {
        jclass isa = env->FindClass("java/lang/String");
        G.String.isa = (jclass) env->NewGlobalRef(isa);
        env->DeleteLocalRef(isa);
    }

    {
        jclass isa = env->FindClass("cn/kkmofang/ker/JSObject");
        G.JSObject.isa = (jclass) env->NewGlobalRef(isa);
        G.JSObject.ptr = env->GetMethodID(isa,"ptr","()J");
        env->DeleteLocalRef(isa);
    }

    {
        jclass isa = env->FindClass("cn/kkmofang/ker/JSONString");
        G.JSONString.isa = (jclass) env->NewGlobalRef(isa);
        G.JSONString.string = env->GetFieldID(isa,"string","Ljava/lang/String;");
        env->DeleteLocalRef(isa);
    }

    {
        jclass isa = env->FindClass("cn/kkmofang/ker/KerHttp");
        G.Http.isa = (jclass) env->NewGlobalRef(isa);
        G.Http.send = env->GetStaticMethodID(isa,"send","(JLjava/lang/String;Ljava/lang/String;Ljava/util/Map;[B)Lcn/kkmofang/ker/http/SessionTask;");
        env->DeleteLocalRef(isa);
    }

    {
        jclass isa = env->FindClass("cn/kkmofang/ker/http/SessionTask");
        G.SessionTask.isa = (jclass) env->NewGlobalRef(isa);
        G.SessionTask.cancel = env->GetMethodID(isa,"cancel","()V");
        env->DeleteLocalRef(isa);
    }

    G.ParameterTypes["java/lang/Integer"] = G.Integer.isa;
    G.ParameterTypes["java/lang/Long"] = G.Long.isa;
    G.ParameterTypes["java/lang/Boolean"] = G.Boolean.isa;
    G.ParameterTypes["java/lang/Float"] = G.Float.isa;
    G.ParameterTypes["java/lang/Double"] = G.Double.isa;
    G.ParameterTypes["java/lang/String"] = G.String.isa;
    G.ParameterTypes["java/lang/Object"] = G.Object.isa;
    G.ParameterTypes["cn/kkmofang/ker/JSObject"] = G.JSObject.isa;

}
