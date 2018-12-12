//
// Created by hailong11 on 2018/12/12.
//

#ifndef KER_KERIMAGE_H
#define KER_KERIMAGE_H

#include <jni.h>
#include <ui/ui.h>


namespace kk {

    namespace ui {

        class OSImage : public Image {

        public:

            OSImage(kk::CString src);

            OSImage(jobject object);

            virtual ~OSImage();

            virtual ImageState state();

            virtual kk::Uint width();

            virtual kk::Uint height();

            virtual kk::CString src();

            virtual void setSrc(kk::CString src);

            virtual void copyPixels(void * data);

            virtual Boolean isCopyPixels();

            virtual void setImage(jobject object);

            virtual void setState(ImageState state);

            virtual jobject object();

        protected:
            jobject _object;
            ImageState _state;
            kk::String _src;
            kk::Uint _width;
            kk::Uint _height;
        };

    }

}

#endif //DEMO_KERIMAGE_H
