//
//  timer.h
//  KK
//
//  Created by zhanghailong on 2018/10/31.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef kk_timer_h
#define kk_timer_h

#include <core/container.h>

namespace kk {
    
    class Timer : public Object {
    public:
        Timer(DispatchQueue * queue, kk::Uint64 delay,kk::Uint64 interval);
        virtual ~Timer();
        virtual void setEvent(std::function<void()> && func);
        virtual void resume();
        virtual void cancel();
        static void Openlib();
    protected:
        Strong<DispatchSource> _source;
    };
    
}

#endif /* timer_h */
