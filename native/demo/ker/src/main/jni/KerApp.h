//
// Created by zhanghailong on 2018/12/13.
//

#ifndef KER_KERAPP_H
#define KER_KERAPP_H

#include <jni.h>
#include <ui/app.h>

namespace kk {

    namespace ui {

        class KerApp : public kk::ui::App {
        public:

            KerApp(jweak object,kk::CString basePath,kk::CString userAgent,kk::CString appkey);

            virtual ~KerApp();

            virtual jweak object();

        protected:
            jweak _object;
        };

    }
}


#endif //DEMO_KERAPP_H
