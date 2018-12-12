//
// Created by hailong11 on 2018/12/12.
//

#ifndef KER_KERVIEW_H
#define KER_KERVIEW_H

#include <jni.h>
#include <ui/view.h>

namespace kk {


    namespace ui {

        class OSView : public View {
        public:

            OSView(jobject view,ViewConfiguration * configuration,Context * context);

            virtual ~OSView() ;

            virtual void set(kk::CString name,kk::CString value) ;

            virtual void setFrame(Rect & frame) ;

            virtual void setContentSize(Size & size) ;

            virtual void setContentOffset(Point & offset,kk::Boolean animated) ;

            virtual Point contentOffset() ;

            virtual kk::Strong<Canvas> createCanvas(Worker * worker) ;

            virtual void addSubview(View * view,SubviewPosition position) ;

            virtual void removeView() ;

            virtual void removeAllSubviews() ;

            virtual void evaluateJavaScript(kk::CString code) ;

            virtual void setAttributedText(AttributedText * text) ;

            virtual void doLoadImage() ;

            virtual void setImage(Image * image);

            virtual void setGravity(kk::CString gravity) ;

            virtual void setContent(kk::CString content,kk::CString contentType,kk::CString basePath);

            virtual jobject object();

        protected:
            jobject _view;
            kk::Strong<kk::TFunction<void, kk::CString, Event *>> _onImageLoadFunc;
        };

    }

}


#endif //DEMO_KERVIEW_H
