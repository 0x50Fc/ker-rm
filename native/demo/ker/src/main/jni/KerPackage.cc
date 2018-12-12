//
// Created by hailong11 on 2018/12/12.
//

#include "KerPackage.h"

namespace kk {

    namespace ui {

        OSPackage::OSPackage(Context * context,kk::CString URI,kk::CString basePath,kk::CString appkey):Package(context,URI),_basePath(basePath),_appkey(appkey) {

        }

        void OSPackage::run(kk::Object * query) {

        }

        kk::Strong<Package> createPackage(Context * context,kk::CString URI) {
            return nullptr;
        }

    }

}