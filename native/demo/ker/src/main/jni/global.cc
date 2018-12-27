//
// Created by zhanghailong on 2018/12/27.
//

#include "global.h"

struct _Global G = {0};

void globalInit(JNIEnv  * env) {

    {
        jclass isa = env->FindClass("cn/kkmofang/ker/Native");
        G.Native.isa = (jclass) env->NewGlobalRef(isa);
        G.Native.gc = env->GetStaticMethodID(isa,"gc","()V");
        G.Native.allocJSObject = env->GetStaticMethodID(isa,"allocJSObject","(J)Lcn/kkmofang/ker/JSObject;");
        G.Native.getImageWidth = env->GetStaticMethodID(isa,"getImageWidth","(Ljava/lang/Object;)I");
        G.Native.getImageHeight = env->GetStaticMethodID(isa,"getImageHeight","(Ljava/lang/Object;)I");
        G.Native.getImage = env->GetStaticMethodID(isa,"getImage","(Lcn/kkmofang/ker/App;JLjava/lang/String;Ljava/lang/String;J)V");
        G.Native.pushObject = env->GetStaticMethodID(isa,"pushObject","(JLjava/lang/Object;)V");
        G.Native.toObject = env->GetStaticMethodID(isa,"toObject","(JI)Ljava/lang/Object;");
        G.Native.getPrototype = env->GetStaticMethodID(isa,"getPrototype","(Ljava/lang/Object;)Ljava/lang/String;");
        G.Native.runApp = env->GetStaticMethodID(isa,"runApp","(Lcn/kkmofang/ker/App;Ljava/lang/String;Ljava/lang/Object;)V");
        G.Native.getPackage = env->GetStaticMethodID(isa,"getPackage","(Lcn/kkmofang/ker/App;JLjava/lang/String;)V");
        G.Native.appendText = env->GetStaticMethodID(isa,"appendText","(Landroid/text/SpannableStringBuilder;Ljava/lang/String;Ljava/lang/String;FZZI)V");
        G.Native.appendImage = env->GetStaticMethodID(isa,"appendImage","(Landroid/text/SpannableStringBuilder;Ljava/lang/Object;IIIIII)V");
        G.Native.viewObtain = env->GetStaticMethodID(isa,"viewObtain","(Ljava/lang/Object;J)V");
        G.Native.viewRecycle = env->GetStaticMethodID(isa,"viewRecycle","(Ljava/lang/Object;J)V");
        G.Native.viewSetAttribute = env->GetStaticMethodID(isa,"viewSetAttribute","(Ljava/lang/Object;JLjava/lang/String;Ljava/lang/String;)V");
        G.Native.viewSetFrame = env->GetStaticMethodID(isa,"viewSetFrame","(Ljava/lang/Object;JFFFF)V");
        G.Native.viewSetContentSize = env->GetStaticMethodID(isa,"viewSetContentSize","(Ljava/lang/Object;JFF)V");
        G.Native.viewSetContentOffset = env->GetStaticMethodID(isa,"viewSetContentOffset","(Ljava/lang/Object;JFFZ)V");
        G.Native.viewGetContentOffsetX = env->GetStaticMethodID(isa,"viewGetContentOffsetX","(Ljava/lang/Object;J)F");
        G.Native.viewGetContentOffsetY = env->GetStaticMethodID(isa,"viewGetContentOffsetY","(Ljava/lang/Object;J)F");
        G.Native.viewAddSubview = env->GetStaticMethodID(isa,"viewAddSubview","(Ljava/lang/Object;JLjava/lang/Object;I)V");
        G.Native.viewRemoveView = env->GetStaticMethodID(isa,"viewRemoveView","(Ljava/lang/Object;J)V");
        G.Native.viewEvaluateJavaScript = env->GetStaticMethodID(isa,"viewEvaluateJavaScript","(Ljava/lang/Object;JLjava/lang/String;)V");
        G.Native.viewSetAttributedText = env->GetStaticMethodID(isa,"viewSetAttributedText","(Ljava/lang/Object;JLjava/lang/CharSequence;)V");
        G.Native.viewSetImage = env->GetStaticMethodID(isa,"viewSetImage","(Ljava/lang/Object;JLjava/lang/Object;)V");
        G.Native.viewSetGravity = env->GetStaticMethodID(isa,"viewSetGravity","(Ljava/lang/Object;JLjava/lang/String;)V");
        G.Native.viewSetContent = env->GetStaticMethodID(isa,"viewSetContent","(Ljava/lang/Object;JLjava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
        G.Native.createView = env->GetStaticMethodID(isa,"createView","(Lcn/kkmofang/ker/App;Ljava/lang/String;J)Ljava/lang/Object;");
        G.Native.getAttributedTextSize = env->GetStaticMethodID(isa,"getAttributedTextSize","(Ljava/lang/CharSequence;F)[F");
        env->DeleteLocalRef(isa);
    }


    {
        jclass isa = env->FindClass("cn/kkmofang/ker/JSFunction");
        G.JSFunction.isa = (jclass) env->NewGlobalRef(isa);
        G.JSFunction.invoke = env->GetMethodID(isa,"invoke","(J)I");
        env->DeleteLocalRef(isa);
    }

    {
        jclass isa = env->FindClass("cn/kkmofang/ker/App");
        G.App.isa = (jclass) env->NewGlobalRef(isa);
        G.App.open = env->GetMethodID(isa,"open","(Ljava/lang/String;Z)V");
        G.App.back = env->GetMethodID(isa,"back","(IZ)V");
        env->DeleteLocalRef(isa);
    }

    {
        jclass isa = env->FindClass("cn/kkmofang/ker/Page");
        G.Page.isa = (jclass) env->NewGlobalRef(isa);
        G.Page.setOptions = env->GetMethodID(isa,"setOptions","(Ljava/lang/Object;)V");
        G.Page.close = env->GetMethodID(isa,"close","(Z)V");
        env->DeleteLocalRef(isa);
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
        jclass isa = env->FindClass("java/lang/Runnable");
        G.Runnable.isa = (jclass) env->NewGlobalRef(isa);
        G.Runnable.run = env->GetMethodID(isa,"run","()V");
        env->DeleteLocalRef(isa);
    }

    {
        jclass isa = env->FindClass("java/lang/Integer");
        G.Integer.isa = (jclass) env->NewGlobalRef(isa);
        G.Integer.init = env->GetMethodID(isa,"<init>","(I)V");
        env->DeleteLocalRef(isa);
    }

    {
        jclass isa = env->FindClass("java/lang/Long");
        G.Long.isa = (jclass) env->NewGlobalRef(isa);
        G.Long.init = env->GetMethodID(isa,"<init>","(J)V");
        env->DeleteLocalRef(isa);
    }

    {
        jclass isa = env->FindClass("java/lang/Boolean");
        G.Boolean.isa = (jclass) env->NewGlobalRef(isa);
        G.Boolean.init = env->GetMethodID(isa,"<init>","(Z)V");
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
        jclass isa = env->FindClass("java/util/TreeMap");
        G.TreeMap.isa = (jclass) env->NewGlobalRef(isa);
        G.TreeMap.init = env->GetMethodID(isa,"<init>","()V");
        G.TreeMap.put = env->GetMethodID(isa,"put","(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
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

}
