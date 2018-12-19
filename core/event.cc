//
//  event.cpp
//  kk
//
//  Created by zhanghailong on 2018/10/26.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#include <core/event.h>

namespace kk {
    
    Object * Event::data() {
        return _data;
    }
    
    void Event::setData(Object * data) {
        _data = data;
    }
    
    
    Any & Event::returnValue() {
        return _returnValue;
    }
    
    void Event::setReturnValue(Any& v) {
        _returnValue = v;
    }
    
    void Event::Openlib() {
        
        kk::Openlib<>::add([](duk_context * ctx)->void{
            
            kk::PushClass<Event>(ctx, [](duk_context * ctx)->void{
                kk::PutProperty<Event,Object *>(ctx, -1, "data", &Event::data,&Event::setData);
                kk::PutProperty<Event,Any&>(ctx, -1, "returnValue", &Event::returnValue,&Event::setReturnValue);
            });
            
        });
        
    }
    
    void EventEmitter::on(kk::CString name,Object * func) {
        String n = name;
        std::map<String,std::list<Strong<Object>>>::iterator i;
        
        if(kk::CStringHasSuffix(n.c_str(),"*")) {
            n = n.substr(0,n.length()-1);
            i = _prefixs.find(n);
            if(i == _prefixs.end()) {
                _prefixs[n] = {func};
            } else {
                i->second.push_back(func);
            }
        } else {
            i = _events.find(n);
            if(i == _events.end()) {
                _events[name] = {func};
            } else {
                i->second.push_back(func);
            }
        }
    }
    
    void EventEmitter::on(kk::CString name,kk::TFunction<void,kk::CString,Event *> * func) {
        on(name,(Object *) func);
    }
    
    void EventEmitter::on(kk::CString name,JSObject * func) {
        on(name,(Object *) func);
    }
    
    void EventEmitter::off(kk::CString name,Object * func) {
        
        if(name == nullptr) {
            _events.clear();
            _prefixs.clear();
            return;
        }
        
        String n = name;
        std::map<String,std::list<Strong<Object>>>::iterator i;
        
        if(kk::CStringHasSuffix(n.c_str(),"*")) {
            n = n.substr(0,n.length()-1);
            i = _prefixs.find(n);
            if(i != _prefixs.end()) {
                if(func == nullptr) {
                    _prefixs.erase(i);
                } else {
                    auto & vs = i->second;
                    auto n = vs.begin();
                    while(n != vs.end()) {
                        if((*n).get() == func) {
                            n = vs.erase(n);
                            continue;
                        }
                        n ++ ;
                    }
                }
            }
        } else {
            i = _events.find(n);
            if(i != _events.end()) {
                if(func == nullptr) {
                    _events.erase(i);
                } else {
                    auto & vs = i->second;
                    auto n = vs.begin();
                    while(n != vs.end()) {
                        if((*n).get() == func) {
                            n = vs.erase(n);
                            continue;
                        }
                        n ++ ;
                    }
                }
            }
        }
        
    }
    
    void EventEmitter::off(kk::CString name,kk::TFunction<void,kk::CString,Event *> * func) {
        off(name,(Object *) func);
    }
    
    void EventEmitter::off(kk::CString name,JSObject * func) {
        off(name,(Object *) func);
    }
    
    void EventEmitter::off(kk::CString name) {
        off(name,(Object *) nullptr);
    }
    
    void EventEmitter::off() {
        off((kk::CString) nullptr,(Object *) nullptr);
    }
    
    kk::Boolean EventEmitter::has(kk::CString name) {
        
        if( _events.find(name) != _events.end() ) {
            return true;
        }
        
        String n(name);
        auto i = _prefixs.begin();
        while(i != _prefixs.end()) {
            if(kk::CStringHasPrefix(n.c_str(),i->first.c_str())) {
                return true;
            }
            i ++;
        }
        return false;
    }
    
    void EventEmitter::emit(kk::CString name,Event * event) {
        
        std::list<Strong<Object>> vs;
        
        String n(name);
        
        auto i = _events.find(n);
        
        if(i != _events.end()) {
            vs = i->second;
        }
        
        i = _prefixs.begin();
        while(i != _prefixs.end()) {
            if(kk::CStringHasPrefix(n.c_str(),i->first.c_str())) {
                auto &m = i->second;
                auto k = m.begin();
                while(k != m.end()) {
                    vs.push_back(*k);
                    k ++;
                }
            }
            i ++;
        }
        
        
        auto k = vs.begin();
        
        while(k != vs.end()) {
            {
                JSObject * fn = dynamic_cast<JSObject *>((*k).get());
                
                if(fn != nullptr) {
                    
                    fn->invoke<void,Event *,CString>(nullptr, event,name);
                    
                    k ++ ;
                    continue;
                }
            }
            {
                kk::TFunction<void,kk::CString,Event *> * fn = dynamic_cast<kk::TFunction<void,kk::CString,Event *> *>((*k).get());
                
                if(fn != nullptr) {
                    
                    (*fn)(name,event);
                    
                    k ++ ;
                    continue;
                }
            }
            k ++;
        }
    }
    
    void EventEmitter::Openlib() {
        
        kk::Openlib<>::add([](duk_context * ctx)->void{
            
            kk::PushClass<EventEmitter>(ctx, [](duk_context * ctx)->void{
                kk::PutMethod<EventEmitter,void,kk::CString,JSObject *>(ctx, -1, "on", &EventEmitter::on);
                kk::PutMethod<EventEmitter,void,kk::CString,JSObject *>(ctx, -1, "off", &EventEmitter::off);
                kk::PutMethod<EventEmitter,kk::Boolean,kk::CString>(ctx, -1, "has", &EventEmitter::has);
                kk::PutMethod<EventEmitter,void,kk::CString,Event *>(ctx, -1, "emit", &EventEmitter::emit);
            });
            
        });
        
    }
    
    
}
