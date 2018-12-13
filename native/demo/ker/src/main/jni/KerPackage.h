//
// Created by hailong11 on 2018/12/12.
//

#ifndef KER_KERPACKAGE_H
#define KER_KERPACKAGE_H


#include <jni.h>
#include <ui/package.h>

namespace kk {

    namespace ui {

        class OSPackage : public Package {
        public:

            OSPackage(Context * context,kk::CString URI);

            virtual void run(kk::Object * query);

        };

    }

}

#endif //DEMO_KERPACKAGE_H
