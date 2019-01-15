//
// Created by zhanghailong on 2018/12/11.
//


#include <core/dispatch.h>
#include <event.h>
#include <queue>

namespace kk {


    static pthread_key_t kGCDDispatchQueueKey = 0;

    static void * LibeventDispatchQueueRunnable(void * userData);
    static void LibeventDispatchQueueLoop(evutil_socket_t fd, short ev, void * userData);

    class LibeventDispatchQueue : public DispatchQueue {
    public:

        LibeventDispatchQueue(kk::CString name):DispatchQueue() {
            _base = event_base_new();
            pthread_mutex_init(&_lock, nullptr);
            _event = evtimer_new(_base,LibeventDispatchQueueLoop,this);
            struct timeval tv = {0,17 * 1000};
            evtimer_add(_event,&tv);
            pthread_create(&_pid, nullptr,LibeventDispatchQueueRunnable,this);
        }

        virtual ~LibeventDispatchQueue(){
            event_base_loopbreak(_base);
            pthread_join(_pid, nullptr);
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

            if((DispatchQueue *) this == getCurrentDispatchQueue()) {
                func();
            } else {
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
                tv.tv_usec = 17 * 1000;
            }

            evtimer_add(_event,&tv);
        }

    protected:

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

        if(kGCDDispatchQueueKey == 0) {
            pthread_key_create(&kGCDDispatchQueueKey, nullptr);
        }

        pthread_setspecific(kGCDDispatchQueueKey,queue);

        event_base * base = queue->base();
        event_base_dispatch(base);
        return nullptr;
    }

    DispatchQueue * getCurrentDispatchQueue() {
        if(kGCDDispatchQueueKey != 0) {
            return (DispatchQueue *) pthread_getspecific(kGCDDispatchQueueKey);
        }
        return nullptr;
    }

    kk::Strong<DispatchQueue> createDispatchQueue(kk::CString name) {
        return new LibeventDispatchQueue(name);
    }


    static void LibeventDispatchSourceCB(evutil_socket_t fd, short ev, void * userData) ;

    class LibeventDispatchSource : public DispatchSource {
    public:

        LibeventDispatchSource(kk::Uint64 fd,DispatchSourceType type,LibeventDispatchQueue * queue):
                _queue(queue),_fd(nullptr),_isResume(false),_type(type) {

            _tv.tv_usec = 0;
            _tv.tv_sec = 0;
            _interval.tv_sec = 0;
            _interval.tv_usec = 0;

            if(type == DispatchSourceTypeTimer) {
                _fd = event_new(queue->base(),-1,0,LibeventDispatchSourceCB,this);
            } else if(type == DispatchSourceTypeRead) {
                _fd = event_new(queue->base(),fd,EV_READ,LibeventDispatchSourceCB,this);
            } else if(type == DispatchSourceTypeWrite) {
                _fd = event_new(queue->base(),fd,EV_WRITE,LibeventDispatchSourceCB,this);
            }

        }

        virtual ~LibeventDispatchSource() {

            if(_fd != nullptr) {
                if(_isResume) {
                    event_del(_fd);
                }
                event_free(_fd);
            }
        }

        virtual void suspend() {
            if(_fd != nullptr && _isResume) {
                event_del(_fd);
                _isResume = false;
            }
        }

        virtual void resume() {
            if(_fd != nullptr && !_isResume) {
                event_add(_fd,&_tv);
                _isResume = true;
            }
        }

        virtual void cancel() {
            if(_fd != nullptr) {
                if(_isResume) {
                    event_del(_fd);
                }
                event_free(_fd);
                _fd = nullptr;
            }
            _event = nullptr;
        }

        virtual void setTimer(kk::Uint64 delay,kk::Uint64 interval) {
            _tv.tv_sec = delay / 1000LL;
            _tv.tv_usec = (delay % 1000LL) * 1000LL;
            _interval.tv_sec = interval / 1000LL;
            _interval.tv_usec = (interval % 1000LL) * 1000LL;
        }

        virtual void setEvent(std::function<void()> && func) {
            _event = func;
        }

        virtual void onEvent() {

            if(_event != nullptr) {

                retain();

                std::function<void()> fn = _event;

                fn();

                if(_fd != nullptr && _isResume) {
                    event_add(_fd,&_interval);
                }

                release();

            }

        }

    protected:

        DispatchSourceType _type;
        kk::Strong<LibeventDispatchQueue> _queue;

        std::function<void()> _event;
        ::event * _fd;
        kk::Boolean _isResume;
        ::timeval _tv;
        ::timeval _interval;
    };

    static void LibeventDispatchSourceCB(evutil_socket_t fd, short ev, void * userData) {
        LibeventDispatchSource * s = (LibeventDispatchSource *) userData;
        s->onEvent();
    }

    kk::Strong<DispatchSource> createDispatchSource(kk::Uint64 fd,DispatchSourceType type,DispatchQueue * queue) {
        return new LibeventDispatchSource(fd, type, (LibeventDispatchQueue *) queue);
    }

    DispatchQueue * IODispatchQueue() {
        static DispatchQueue * v = nullptr;
        if(v == nullptr) {
            v = new LibeventDispatchQueue("kk::IODispatchQueue");
            v->retain();
        }
        return v;
    }

    DispatchQueue * NetDispatchQueue() {
        static DispatchQueue * v = nullptr;
        if(v == nullptr) {
            v = new LibeventDispatchQueue("kk::NetDispatchQueue");
            v->retain();
        }
        return v;
    }

    struct event_base * GetDispatchQueueEventBase(DispatchQueue * queue) {
        return ((LibeventDispatchQueue *) queue)->base();
    }

    kk::Object * getDispatchQueueSpecific(const void * key) {
        DispatchQueue * queue = getCurrentDispatchQueue();
        if(queue != nullptr) {
            return queue->getSpecific(key);
        }
        return nullptr;
    }

    void setDispatchQueueSpecific(const void * key,kk::Object * object) {
        DispatchQueue * queue = getCurrentDispatchQueue();
        assert(queue != nullptr);
        queue->setSpecific(key,object);
    }

}

