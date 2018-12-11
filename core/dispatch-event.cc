//
// Created by zhanghailong on 2018/12/11.
//


#include <core/dispatch.h>
#include <event.h>

namespace kk {

    class LibeventDispatchQueue : public DispatchQueue {
    public:

        LibeventDispatchQueue(kk::CString name,DispatchQueueType type) {

        }

        LibeventDispatchQueue() {

        }

        virtual ~LibeventDispatchQueue(){
            event_base_free(_base);
        }

        virtual void async(std::function<void()> && func) {

        }
        virtual void sync(std::function<void()> && func) {

        }
        virtual struct event_base * base() {
            return _base;
        }
    protected:
        struct event_base * _base;
    };


    kk::Strong<DispatchQueue> createDispatchQueue(kk::CString name,DispatchQueueType type) {
        return new LibeventDispatchQueue(name,type);
    }

    DispatchQueue * mainDispatchQueue() {
        static DispatchQueue * v = nullptr;
        if(v == nullptr){
            v = new LibeventDispatchQueue();
            v->retain();
        }
        return v;
    }

    class LibeventDispatchSource : public DispatchSource {
    public:

        LibeventDispatchSource(kk::Uint64 fd,DispatchSourceType type,LibeventDispatchQueue * queue) {


        }

        virtual ~GCDDispatchSource() {

        }

        virtual void suspend() {

        }

        virtual void resume() {

        }

        virtual void cancel() {

        }

        virtual void setTimer(kk::Uint64 delay,kk::Uint64 interval) {

        }

        virtual void setEvent(std::function<void()> && func) {
            _event = func;
        }

    protected:

        virtual void onEvent() {
            if(_event != nullptr) {
                std::function<void()> fn = _event;
                fn();
            }
        }

        std::function<void()> _event;
    };

    kk::Strong<DispatchSource> createDispatchSource(kk::Uint64 fd,DispatchSourceType type,DispatchQueue * queue) {
        return new LibeventDispatchSource(fd, type, (LibeventDispatchQueue *) queue);
    }

    DispatchQueue * IODispatchQueue() {
        static DispatchQueue * v = nullptr;
        if(v == nullptr) {
            
        }
        static dispatch_once_t onceToken;
        dispatch_once(&onceToken, ^{
            v = new kk::GCDDispatchQueue("kk::IODispatchQueue",DispatchQueueTypeSerial);
            v->retain();
        });
        return v;
    }

    ::dispatch_queue_t DispatchQueueGCD(DispatchQueue * queue) {
        return ((GCDDispatchQueue *) queue)->queue();
    }

}

