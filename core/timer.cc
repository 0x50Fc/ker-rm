//
//  timer.cc
//  KK
//
//  Created by zhanghailong on 2018/10/31.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#include <core/timer.h>

namespace kk {
    
    
    Timer::Timer(DispatchQueue * queue, kk::Uint64 delay,kk::Uint64 interval) {
        _source = createDispatchSource(0, DispatchSourceTypeTimer, queue);
        _source->setTimer(delay, interval);

    }
    
    void Timer::setEvent(std::function<void()> && func) {
        if(_source) {
            _source->setEvent(std::move(func));
        }
    }
    
    Timer::~Timer() {
        if(_source) {
            _source->cancel();
        }
        kk::Log("[Timer] [dealloc]");
    }
    
    void Timer::resume() {
        if(_source) {
            _source->resume();
        }
    }
    
    void Timer::cancel() {
        if(_source) {
            _source->cancel();
            _source = nullptr;
        }
    }
    
    void Timer::Openlib() {
        
        kk::Openlib<Container *>::add([](duk_context * ctx, Container * container)->void {
            
            Weak<Object> s = dynamic_cast<Object *>(container);
            
            PushFunction(ctx, new TFunction<kk::Uint64, JSObject *,kk::Int>([s](JSObject * func,kk::Int tv)->kk::Uint64{
                
                kk::Strong<Object> o = s.operator->();
                
                Container * container = dynamic_cast<Container *>(o.get());
                
                if(container != nullptr) {
                    kk::Strong<JSObject> fn = func;
                    Timer * v = new Timer(container->queue(),tv,0);
                    v->setEvent([fn,container,v]()->void{
                        fn->invoke<void>(nullptr);
                        v->cancel();
                        container->remove(v);
                    });
                    container->set(v);
                    v->resume();
                    return (kk::Uint64) v;
                }
                
                return 0;
            }));
            
            duk_put_global_string(ctx, "setTimeout");
            
            PushFunction(ctx, new TFunction<void, kk::Uint64>([s](kk::Uint64 id )->void{
                
                kk::Strong<Object> o = s.operator->();
                
                Container * container = dynamic_cast<Container *>(o.get());
                
                if(container != nullptr && id != 0) {
                    
                    Timer * v = dynamic_cast<Timer *>(container->get((kk::Object *) id));
                    
                    if(v != nullptr) {
                        v->cancel();
                        container->remove(v);
                    }
                    
                }
                
            }));
        
            duk_put_global_string(ctx, "clearTimeout");
            
            PushFunction(ctx, new TFunction<kk::Uint64, JSObject *,kk::Int>([s](JSObject * func,kk::Int tv)->kk::Uint64{
                
                kk::Strong<Object> o = s.operator->();
                
                Container * container = dynamic_cast<Container *>(o.get());
                
                if(container != nullptr) {
                    kk::Strong<JSObject> fn = func;
                    Timer * v = new Timer(container->queue(),tv,tv);
                    v->setEvent([fn]()->void{
                        kk::Strong<JSObject> & func = (kk::Strong<JSObject> &) fn;
                        func->invoke<void>(nullptr);
                    });
                    container->set(v);
                    v->resume();
                    return (kk::Uint64) v;
                }
                
                return 0;
            }));
            
            duk_put_global_string(ctx, "setInterval");
            
            PushFunction(ctx, new TFunction<void, kk::Uint64>([s](kk::Uint64 id )->void{
                
                kk::Strong<Object> o = s.operator->();
                
                Container * container = dynamic_cast<Container *>(o.get());
                
                if(container != nullptr && id != 0) {
                    
                    Timer * v = dynamic_cast<Timer *>(container->get((kk::Object *) id));
                    
                    if(v != nullptr) {
                        v->cancel();
                        container->remove(v);
                    }
                    
                }
                
            }));
            
            duk_put_global_string(ctx, "clearInterval");
            
            
        });
        
    }
    
}
