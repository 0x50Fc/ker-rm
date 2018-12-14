//
// Created by zhanghailong on 2018/12/14.
//

#ifndef KER_KERPAGE_H
#define KER_KERPAGE_H

#include <jni.h>
#include <ui/page.h>

namespace kk {

    namespace ui {

        class KerPage : public Page {
        public:
            KerPage(App * app,View * view,jweak object);
            virtual ~KerPage();
            virtual jweak object();

        protected:
            jweak _object;
        };
    }

}

#endif //DEMO_KERPAGE_H
