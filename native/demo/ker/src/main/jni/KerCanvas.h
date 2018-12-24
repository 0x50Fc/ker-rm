//
// Created by hailong11 on 2018/12/12.
//

#ifndef KER_KERCANVAS_H
#define KER_KERCANVAS_H

#include <jni.h>
#include <ui/view.h>

namespace kk {

    namespace ui {

        class OSCanvas : public Canvas {
        public:

            OSCanvas(DispatchQueue * queue);

            OSCanvas(DispatchQueue * queue, jobject object, kk::Uint width,kk::Uint height);

            virtual ~OSCanvas();

            virtual Strong<Object> getContext(kk::CString name) ;

            virtual Uint width();

            virtual void setWidth(Uint v);

            virtual Uint height();

            virtual void setHeight(Uint v);

            virtual Strong<Image> toImage();

        protected:
            jobject _object;
            kk::Uint _width;
            kk::Uint _height;
            kk::Boolean _resize;
            kk::Strong<Object> _context;
        };


    }

}


#endif //DEMO_KERCANVAS_H
