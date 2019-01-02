//
// Created by hailong11 on 2018/12/12.
//

#include "KerView.h"
#include "kk.h"
#include "KerImage.h"
#include "KerApp.h"

namespace kk {

    namespace ui {

        jobject getAttributedTextString(JNIEnv * env, AttributedText * text) {

            jclass isa = env->FindClass("android/text/SpannableStringBuilder");
            jmethodID init = env->GetMethodID(isa,"<init>","()V");
            jobject v = env->NewObject(isa,init);

            jclass native = env->FindClass("cn/kkmofang/ker/Native");

            //public static void appendText(SpannableStringBuilder string,String text,String family,float size,boolean bold,boolean italic,int color)
            jmethodID appendText = env->GetStaticMethodID(native,"appendText","(Landroid/text/SpannableStringBuilder;Ljava/lang/String;Ljava/lang/String;FZZI)V");

            //public static void appendImage(SpannableStringBuilder string,Object image,int width,int height,int left,int top,int right,int bottom)
            jmethodID appendImage = env->GetStaticMethodID(native,"appendImage","(Landroid/text/SpannableStringBuilder;Ljava/lang/Object;IIIIII)V");

            if(text != nullptr) {
                auto& m = text->spans();
                auto i = m.begin();
                while(i != m.end()) {
                    AttributedTextSpan & span = *i;
                    if(span.type == AttributedTextSpanTypeText) {
                        jstring family = env->NewStringUTF(span.font.family.c_str());
                        jstring text = env->NewStringUTF(span.text.c_str());
                        env->CallStaticVoidMethod(native,
                                                  appendText,
                                                  v,
                                                  text,
                                                  family,
                                                  (jfloat)span.font.size,
                                                  (jboolean)(span.font.weight == FontWeightBold),
                                                  (jboolean)(span.font.style == FontStyleItalic),
                                                  (jint)(span.color.intValue()));
                        env->DeleteLocalRef(family);
                        env->DeleteLocalRef(text);
                    } else if(span.type == AttributedTextSpanTypeImage) {
                        kk::ui::OSImage * image = span.image;
                        if(image != nullptr && image->object() != nullptr) {
                            env->CallStaticVoidMethod(native,
                                                      appendImage,
                                                      v,
                                                      image->object(),
                                                      (jint)span.width,
                                                      (jint)span.height,
                                                      (jint)span.margin.left,
                                                      (jint)span.margin.top,
                                                      (jint)span.margin.right,
                                                      (jint)span.margin.bottom);
                        }
                    }
                    i ++;
                }
            }

            env->DeleteLocalRef(isa);
            env->DeleteLocalRef(native);

            return v;
        }

        Size getAttributedTextContentSize(Context * context,AttributedText * text,kk::Float maxWidth) {

            jboolean isAttach = false;

            JNIEnv *env = kk_env(&isAttach);

            jobject v = getAttributedTextString(env,text);

            jclass isa = env->FindClass("cn/kkmofang/ker/Native");

            //public static float[] getAttributedTextSize(CharSequence string,float maxWidth)
            jmethodID getAttributedTextSize = env->GetStaticMethodID(isa,"getAttributedTextSize","(Ljava/lang/CharSequence;F)[F");

            jobject r = env->CallStaticObjectMethod(isa,getAttributedTextSize,v,(jfloat) maxWidth);

            if(v != nullptr) {
                env->DeleteLocalRef(v);
            }

            env->DeleteLocalRef(isa);

            Size rr;

            if(r != nullptr) {

                jint n = env->GetArrayLength((jfloatArray) r);
                jfloat * p = env->GetFloatArrayElements((jfloatArray)r, nullptr);

                if(n > 0) {
                    rr.width = p[0];
                }

                if(n > 1) {
                    rr.height = p[1];
                }

                env->ReleaseFloatArrayElements((jfloatArray)r,p,0);

                env->DeleteLocalRef(r);

            }

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }

            return rr;
        }

    }

}
