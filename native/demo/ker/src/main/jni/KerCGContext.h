//
// Created by zhanghailong on 2018/12/18.
//

#ifndef KER_KERCGCONTEXT_H
#define KER_KERCGCONTEXT_H

#include <jni.h>
#include <ui/CGContext.h>

namespace kk {

    namespace ui {


        void displayCGContext(kk::ui::CG::Context * context,jlong viewId,jlong appid);

    }

}



#endif //DEMO_KERCGCONTEXT_H
