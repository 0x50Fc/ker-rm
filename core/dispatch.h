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
        DispatchQueue():_autoId(0){}
        virtual void async(std::function<void()> && func) = 0;
        virtual void sync(std::function<void()> && func) = 0;
        virtual void setSpecific(const void * key,kk::Object * object) {
            _specifics[key] = object;
        }
        virtual kk::Object * getSpecific(const void * key) {
            auto i = _specifics.find(key);
            if(i != _specifics.end()) {
                return i->second;
            }
            return nullptr;
        }
        virtual kk::Object * get(kk::Uint64 key) {
            {
                auto i = _strongs.find(key);
                if(i != _strongs.end()) {
                    return i->second;
                }
            }
            {
                auto i = _weaks.find(key);
                if(i != _weaks.end()) {
                    kk::Object * v = i->second;
                    if(v == nullptr) {
                        _weaks.erase(i);
                    }
                    return v;
                }
            }
            return nullptr;
        }
        virtual kk::Uint64 strong(kk::Object * object) {
            kk::Uint64 key = ++ _autoId;
            _strongs[key] = object;
            return key;
        }
        virtual kk::Uint64 weak(kk::Object * object) {
            kk::Uint64 key = ++ _autoId;
            _weaks[key] = object;
            return key;
        }
        virtual void remove(kk::Uint64 key) {
            {
                auto i = _strongs.find(key);
                if(i != _strongs.end()) {
                    _strongs.erase(i);
                }
            }
            {
                auto i = _weaks.find(key);
                if(i != _weaks.end()) {
                    _weaks.erase(i);
                }
            }
        }
    protected:
        kk::Uint64 _autoId;
        std::map<const void * ,kk::Strong<kk::Object>> _specifics;
        std::map<kk::Uint64,kk::Strong<kk::Object>> _strongs;
        std::map<kk::Uint64,kk::Weak<kk::Object>> _weaks;
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
    
    DispatchQueue * IODispatchQueue();
    
    DispatchQueue * getCurrentDispatchQueue();
    
    kk::Object * getDispatchQueueSpecific(const void * key);
    
    void setDispatchQueueSpecific(const void * key,kk::Object * object);
    
}

#endif /* dispatch_h */
