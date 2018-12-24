//
// Created by zhanghailong on 2018/12/18.
//

#include "KerCGContext.h"
#include "kk.h"

namespace kk {

    namespace ui {

        OSCGContext::OSCGContext(JNIEnv * env,jobject object,kk::Uint width,kk::Uint height):
                _width(width),_height(height),
                _lineWidth(0),_textAlign(TextAlignLeft),
                _shadowBlur(0),_shadowOffsetX(0),_shadowOffsetY(0),
                _baseline(TextBaselineAlphabetic),_lineCap(CG::LineCapTypeButt),_lineJoin(CG::LineJoinTypeMiter),_miterLimit(0),
                _globalAlpha(1),_globalCompositeOperation(CG::GlobalCompositeOperationSourceOver){
            _object = env->NewGlobalRef(object);
        }

        OSCGContext::~OSCGContext() {

            jboolean isAttach = false;

            JNIEnv *env = kk_env(&isAttach);

            env->DeleteGlobalRef(_object);

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }

        }

        jobject OSCGContext::object() {
            return _object;
        }

        kk::Strong<CG::LinearGradient> OSCGContext::createLinearGradient(Float x0,Float y0, Float x1, Float y1) {
            return nullptr;
        }

        kk::Strong<CG::RadialGradient> OSCGContext::createRadialGradient(Float x0,Float y0,Float r0, Float x1, Float y1, Float r1) {
            return nullptr;
        }

        kk::Strong<CG::Pattern> OSCGContext::createPattern(Image * image,CG::PatternType type) {
            return nullptr;
        }

        kk::Strong<CG::ImageData> OSCGContext::createImageData(Uint width,Uint height) {
            return nullptr;
        }

        kk::Strong<CG::ImageData> OSCGContext::getImageData(Uint x,Uint y,Uint width,Uint height) {
            return nullptr;
        }

        void OSCGContext::putImageData(CG::ImageData * data,Uint x,Uint y,Uint dirtyX,Uint dirtyY,Uint dirtyWidth,Uint dirtyHeight) {

        }

        void OSCGContext::save() {

            jboolean isAttach = false;

            JNIEnv *env = kk_env(&isAttach);

            jclass isa = env->GetObjectClass(_object);

            jmethodID save = env->GetMethodID(isa,"save","()I");

            env->CallIntMethod(_object,save);

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }

        }

        void OSCGContext::restore(){

            jboolean isAttach = false;

            JNIEnv *env = kk_env(&isAttach);

            jclass isa = env->GetObjectClass(_object);

            jmethodID restore = env->GetMethodID(isa,"restore","()V");

            env->CallVoidMethod(_object,restore);

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }

        }

        void OSCGContext::rect(Float x, Float y,Float width,Float height) {

        }

        void OSCGContext::fillRect(Float x, Float y,Float width,Float height) {

        }

        void OSCGContext::strokeRect(Float x, Float y,Float width,Float height) {

        }

        void OSCGContext::clearRect(Float x, Float y,Float width,Float height) {

            jboolean isAttach = false;

            JNIEnv *env = kk_env(&isAttach);

            jclass isa = env->GetObjectClass(_object);

            jmethodID clearRect = env->GetMethodID(isa,"clearRect","(IIII)V");

            env->CallVoidMethod(_object,clearRect,(jint) x,(jint) y,(jint) width,(jint) height);

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }

        }

        void OSCGContext::fill() {

            jboolean isAttach = false;

            JNIEnv *env = kk_env(&isAttach);

            jclass isa = env->GetObjectClass(_object);

            jmethodID fill = env->GetMethodID(isa,"fill","()V");

            env->CallVoidMethod(_object,fill);

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }

        }

        void OSCGContext::stroke() {

            jboolean isAttach = false;

            JNIEnv *env = kk_env(&isAttach);

            jclass isa = env->GetObjectClass(_object);

            jmethodID stroke = env->GetMethodID(isa,"stroke","()V");

            env->CallVoidMethod(_object,stroke);

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }

        }

        void OSCGContext::beginPath() {

            jboolean isAttach = false;

            JNIEnv *env = kk_env(&isAttach);

            jclass isa = env->GetObjectClass(_object);

            jmethodID beginPath = env->GetMethodID(isa,"beginPath","()V");

            env->CallVoidMethod(_object,beginPath);

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }
        }

        void OSCGContext::moveTo(Float x,Float y) {

        }

        void OSCGContext::closePath() {

        }

        void OSCGContext::lineTo(Float x,Float y) {

        }

        void OSCGContext::clip() {

        }

        void OSCGContext::quadraticCurveTo(Float cpx,Float cpy,Float x,Float y) {

        }

        void OSCGContext::bezierCurveTo(Float cp1x,Float cp1y,Float cp2x,Float cp2y,Float x,Float y) {

        }

        void OSCGContext::arc(Float x,Float y,Float r, Float sAngle,Float eAngle,Boolean counterclockwise) {

            jboolean isAttach = false;

            JNIEnv *env = kk_env(&isAttach);

            jclass isa = env->GetObjectClass(_object);

            jmethodID arc = env->GetMethodID(isa,"arc","(FFFFFZ)V");

            env->CallVoidMethod(_object,arc,x,y,r,sAngle,eAngle,counterclockwise);

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }

        }

        void OSCGContext::arcTo(Float x1,Float y1,Float x2,Float y2,Float r) {

        }

        Boolean OSCGContext::isPointInPath(Float x,Float y) {

        }

        void OSCGContext::scale(Float sx,Float sy) {

        }

        void OSCGContext::rotate(Float angle) {

        }

        void OSCGContext::translate(Float dx,Float dy) {

        }

        void OSCGContext::transform(Float a,Float b,Float c,Float d,Float e,Float f) {

        }

        void OSCGContext::setTransform(Float a,Float b,Float c,Float d,Float e,Float f) {

        }

        void OSCGContext::fillText(kk::CString text,Float x,Float y,Float maxWidth) {

        }

        void OSCGContext::strokeText(kk::CString text,Float x,Float y,Float maxWidth) {

        }

        Float OSCGContext::measureText(kk::CString text) {

        }

        void OSCGContext::drawImage(Image * image,Float sx,Float sy,Float swidth,Float sheight,Float x,Float y,Float width,Float height) {

        }

        void OSCGContext::setFillStyle(CG::Style * style) {
            _fillStyle = style;
        }

        CG::Style * OSCGContext::fillStyle() {
            return _fillStyle;
        }

        void OSCGContext::setFillColor(Color color) {
            _fillColor = color;

            jboolean isAttach = false;

            JNIEnv *env = kk_env(&isAttach);

            jclass isa = env->GetObjectClass(_object);

            jmethodID setFillColor = env->GetMethodID(isa,"setFillColor","(I)V");

            env->CallVoidMethod(_object,setFillColor,(jint) color.intValue());

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }
        }

        Color OSCGContext::fillColor() {
            return _fillColor;
        }

        void OSCGContext::setStrokeStyle(CG::Style * style) {
            _strokeStyle = style;
        }

        CG::Style * OSCGContext::strokeStyle() {
            return _strokeStyle;
        }

        void OSCGContext::setStrokeColor(Color color) {
            _strokeColor = color;

            jboolean isAttach = false;

            JNIEnv *env = kk_env(&isAttach);

            jclass isa = env->GetObjectClass(_object);

            jmethodID setStrokeColor = env->GetMethodID(isa,"setStrokeColor","(I)V");

            env->CallVoidMethod(_object,setStrokeColor,(jint) color.intValue());

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }

        }

        Color OSCGContext::strokeColor() {
            return _strokeColor;
        }

        void OSCGContext::setShadowColor(Color v) {
            _shadowColor = v;
        }

        Color OSCGContext::shadowColor() {
            return _shadowColor;
        }

        void OSCGContext::setShadowBlur(Float v) {
            _shadowBlur = v;
        }

        Float OSCGContext::shadowBlur() {
            return _shadowBlur;
        }

        void OSCGContext::setShadowOffsetX(Float v) {
            _shadowOffsetX = v;
        }

        Float OSCGContext::shadowOffsetX() {
            return _shadowOffsetX;
        }

        void OSCGContext::setShadowOffsetY(Float v) {
            _shadowOffsetY = v;
        }

        Float OSCGContext::shadowOffsetY() {
            return _shadowOffsetY;
        }

        void OSCGContext::setLineCap(CG::LineCapType v) {
            _lineCap =  v;
        }

        CG::LineCapType OSCGContext::lineCap() {
            return _lineCap;
        }

        void OSCGContext::setLineJoin(CG::LineJoinType v) {
            _lineJoin = v;
        }

        CG::LineJoinType OSCGContext::lineJoin() {
            return _lineJoin;
        }

        void OSCGContext::setLineWidth(Float v) {
            _lineWidth = v;
        }

        Float OSCGContext::lineWidth() {
            return _lineWidth;
        }

        void OSCGContext::setMiterLimit(Float v) {
            _miterLimit = v;
        }

        Float OSCGContext::miterLimit() {
            return _miterLimit;
        }

        void OSCGContext::setFont(Font v) {
            _font = v;
        }

        Font OSCGContext::font() {
            return _font;
        }

        void OSCGContext::setTextAlign(TextAlign v) {
            _textAlign = v;
        }

        TextAlign OSCGContext::textAlign() {
            return _textAlign;
        }

        void OSCGContext::setTextBaseline(TextBaseline v) {
            _baseline = v;
        }

        TextBaseline OSCGContext::textBaseline() {
            return _baseline;
        }

        void OSCGContext::setGlobalAlpha(Float v) {
            _globalAlpha = v;
        }

        Float OSCGContext::globalAlpha() {
            return _globalAlpha;
        }

        void OSCGContext::setGlobalCompositeOperation(CG::GlobalCompositeOperation v) {
            _globalCompositeOperation = v;
        }

        CG::GlobalCompositeOperation OSCGContext::globalCompositeOperation() {
            return _globalCompositeOperation;
        }

        kk::Strong<kk::ui::CG::Context> createCGContext(kk::Uint width,kk::Uint height) {

            kk::Strong<kk::ui::CG::Context> v;

            jboolean isAttach = false;

            JNIEnv *env = kk_env(&isAttach);

            jclass isa = env->FindClass("cn/kkmofang/ker/Native");

            jmethodID createCanvas = env->GetStaticMethodID(isa,"createCanvas","(II)Lcn/kkmofang/ker/KerCanvas;");

            jobject object = env->CallStaticObjectMethod(isa,createCanvas,(jint) width,(jint) height);

            if(object != nullptr) {
                v = new OSCGContext(env,object,width,height);
                env->DeleteLocalRef(object);
            }

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }

            return v;

        }

        void displayCGContext(kk::ui::CG::Context * context,jobject view) {

            OSCGContext * v = dynamic_cast<OSCGContext *>(context);

            if(v == nullptr) {
                return;
            }

            jboolean isAttach = false;

            JNIEnv *env = kk_env(&isAttach);

            jclass isa = env->FindClass("cn/kkmofang/ker/Native");

            jmethodID displayCanvas = env->GetStaticMethodID(isa,"displayCanvas","(Lcn/kkmofang/ker/KerCanvas;Ljava/lang/Object;)V");

            env->CallStaticVoidMethod(isa,displayCanvas, v->object(),view);

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }

        }

    }

}

