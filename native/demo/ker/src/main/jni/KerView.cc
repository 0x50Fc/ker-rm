//
// Created by hailong11 on 2018/12/12.
//

#include "KerView.h"
#include "kk.h"
#include "KerImage.h"
#include "global.h"

namespace kk {

    namespace ui {

        jobject getAttributedTextString(JNIEnv * env, AttributedText * text) {

            jobject v = env->NewObject(G.SpannableStringBuilder.isa,G.SpannableStringBuilder.init);

            if(text != nullptr) {
                auto& m = text->spans();
                auto i = m.begin();
                while(i != m.end()) {
                    AttributedTextSpan & span = *i;
                    if(span.type == AttributedTextSpanTypeText) {
                        jstring family = env->NewStringUTF(span.font.family.c_str());
                        jstring text = env->NewStringUTF(span.text.c_str());
                        env->CallStaticVoidMethod(G.UI.isa,
                                                  G.UI.appendText,
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
                        jobject image = GetImageObject(span.image);
                        if(image != nullptr ) {
                            env->CallStaticVoidMethod(G.UI.isa,
                                                      G.UI.appendImage,
                                                      v,
                                                      image,
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

            return v;
        }

        Size getAttributedTextContentSize(Context * context,AttributedText * text,kk::Float maxWidth) {

            jboolean isAttach = false;

            JNIEnv *env = kk_env(&isAttach);

            jobject v = getAttributedTextString(env,text);

            jobject r = env->CallStaticObjectMethod(G.UI.isa,G.UI.getAttributedTextSize,v,(jfloat) maxWidth);

            if(v != nullptr) {
                env->DeleteLocalRef(v);
            }

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
