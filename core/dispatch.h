//
//  dispatch.h
//  kk
//
//  Created by zhanghailong on 2018/10/26.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef kk_dispatch_h
#define kk_dispatch_h

#include <core/kk.h>
#include <list>



namespace kk {
    
    class DispatchQueue : public Object {
    public:
        virtual void async(std::function<void()> && func) = 0;
        virtual void sync(std::function<void()> && func) = 0;
    };
    
    enum DispatchSourceType {
        DispatchSourceTypeRead,
        DispatchSourceTypeWrite,
        DispatchSourceTypeTimer,
        DispatchSourceTypeSignal
    };
    
    class DispatchSource : public Object {
    public:
        virtual void suspend() = 0;
        virtual void resume() = 0;
        virtual void cancel() = 0;
        virtual void setTimer(kk::Uint64 delay,kk::Uint64 interval) = 0;
        virtual void setEvent(std::function<void()> && func) = 0;
    };
    
    kk::Strong<DispatchQueue> createDispatchQueue(kk::CString name);
    
    kk::Strong<DispatchSource> createDispatchSource(kk::Uint64 fd,DispatchSourceType type,DispatchQueue * queue);
    
    DispatchQueue * mainDispatchQueue();
    
    DispatchQueue * IODispatchQueue();
    
    DispatchQueue * getCurrentDispatchQueue();
    
    
}

#endif /* dispatch_h */
