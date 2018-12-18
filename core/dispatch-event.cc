//
// Created by zhanghailong on 2018/12/11.
//


#include <core/dispatch.h>
#include <event.h>
#include <queue>

namespace kk {


    static void * LibeventDispatchQueueRunnable(void * userData);
    static void LibeventDispatchQueueLoop(evutil_socket_t fd, short ev, void * userData);

    class LibeventDispatchQueue : public DispatchQueue {
    public:

        LibeventDispatchQueue(kk::CString name,DispatchQueueType type) {
            _base = event_base_new();
            pthread_mutex_init(&_lock, nullptr);
            setup();
            pthread_create(&_pid, nullptr,LibeventDispatchQueueRunnable,this);
        }

        LibeventDispatchQueue() {
            _pid = 0;
            _base = event_base_new();
            pthread_mutex_init(&_lock, nullptr);
            setup();
        }

        virtual ~LibeventDispatchQueue(){
            if(_pid != 0) {
                event_base_loopbreak(_base);
                pthread_join(_pid, nullptr);
            }
            evtimer_del(_event);
            event_free(_event);
            pthread_mutex_destroy(&_lock);
            event_base_free(_base);
        }

        virtual void async(std::function<void()> && func) {
            pthread_mutex_lock(&_lock);
            _funcs.push(func);
            pthread_mutex_unlock(&_lock);
        }

        virtual void sync(std::function<void()> && func) {

            pthread_cond_t v;

            pthread_cond_init(&v, nullptr);

            pthread_mutex_lock(&_lock);

            _funcs.push([&func,&v]()->void{
                func();
                pthread_cond_broadcast(&v);
            });

            pthread_cond_wait(&v,&_lock);

            pthread_mutex_unlock(&_lock);

            pthread_cond_destroy(&v);

        }

        virtual struct event_base * base() {
            return _base;
        }

        virtual void loop() {

            struct timeval tv = {0,0};

            pthread_mutex_lock(&_lock);

            std::function<void()> fn;

            if(!_funcs.empty()) {
                fn = _funcs.front();
                _funcs.pop();
            }

            pthread_mutex_unlock(&_lock);

            if(fn != nullptr) {
                fn();
            } else {
                tv.tv_usec = 17 * 1000000;
            }

            evtimer_add(_event,&tv);
        }

    protected:

        virtual void setup() {
            _event = evtimer_new(_base,LibeventDispatchQueueLoop,this);
            struct timeval tv = {0,17 * 1000000};
            evtimer_add(_event,&tv);
        }

        pthread_t _pid;
        struct event_base * _base;
        struct event * _event;
        std::queue<std::function<void()>> _funcs;
        pthread_mutex_t _lock;
    };

    static void LibeventDispatchQueueLoop(evutil_socket_t fd, short ev, void * userData) {
        LibeventDispatchQueue * queue = (LibeventDispatchQueue *) userData;
        queue->loop();
    }

    static void * LibeventDispatchQueueRunnable(void * userData) {
        LibeventDispatchQueue * queue = (LibeventDispatchQueue *) userData;
        event_base * base = queue->base();
        event_base_dispatch(base);
        return nullptr;
    }

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

        virtual ~LibeventDispatchSource() {

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
            v = new LibeventDispatchQueue("kk::IODispatchQueue",DispatchQueueTypeSerial);
        }
        return v;
    }

    struct event_base * GetDispatchQueueEventBase(DispatchQueue * queue) {
        return ((LibeventDispatchQueue *) queue)->base();
    }

}

