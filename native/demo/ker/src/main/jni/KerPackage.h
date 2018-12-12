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

            OSPackage(Context * context,kk::CString URI,kk::CString basePath,kk::CString appkey);

            virtual void run(kk::Object * query);

        protected:
            kk::String _basePath;
            kk::String _appkey;
        };

    }

}

#endif //DEMO_KERPACKAGE_H
