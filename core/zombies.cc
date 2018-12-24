//
//  zombies.cc
//  Ker
//
//  Created by hailong11 on 2018/12/20.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#include <core/kk.h>
#include <pthread.h>
#include <list>
#include <pthread.h>

#ifdef __ANDROID_API__
#include <backtrace/Backtrace.h>
#include <dlfcn.h>
#include <unwind.h>

static void * libbacktrace = nullptr;

#define MIN(a,b) ((a) < (b) ? (a) : (b))

struct BacktraceState
{
    void** current;
    void** end;
};

static _Unwind_Reason_Code unwindCallback(struct _Unwind_Context* context, void* arg)
{
    BacktraceState* state = static_cast<BacktraceState*>(arg);
    uintptr_t pc = _Unwind_GetIP(context);
    if (pc) {
        if (state->current == state->end) {
            return _URC_END_OF_STACK;
        } else {
            *state->current++ = reinterpret_cast<void*>(pc);
        }
    }
    return _URC_NO_REASON;
}


int backtrace(void ** array,int size) {
    BacktraceState state = {array, array + size};
    _Unwind_Backtrace(unwindCallback, &state);
    return state.current - array;
}

char ** backtrace_symbols(void ** array,int size) {
    
    char ** v = nullptr;

    v = (char **) malloc(sizeof(char *) * size);

    for(int i = 0; i < size; i++) {

        const void* addr = array[i];

        Dl_info info;
        if (dladdr(addr, &info) && info.dli_sname) {
            size_t n = strlen(info.dli_sname) + 1;
            v[i] = (char *) malloc(n);
            memcpy(v[i],info.dli_sname,n);
        } else {
            v[i] = nullptr;
        }

    }

    return v;
}



struct ZombieObject {
    char b[32];
    size_t size;
//    char** backtrace_symbols;
//    int backtrace_size;
    char e[32];
};

#define ZombieObjectB "CDCDCDCDCDCDCD"
#define ZombieObjectE "DEDEDEDEDEDEDE"

static char ZombieObjectEnd[] = "DEDEDEDEDEDEDE";

void * operator new(size_t size) {
    struct ZombieObject * v = (struct ZombieObject *) malloc(size + sizeof(struct ZombieObject) + sizeof(ZombieObjectEnd));
    void * array[10];
    strcpy(v->b,ZombieObjectB);
    v->size = size;
//    v->backtrace_size = backtrace(array,10);
//    v->backtrace_symbols = backtrace_symbols(array,v->backtrace_size);
    strcpy(v->e,ZombieObjectE);
    strcpy((char *) (v + 1) + size,ZombieObjectEnd);
    return v + 1;
}

void operator delete(void * p) {
    struct ZombieObject * v = ((struct ZombieObject *) p) - 1;
    assert(strcmp(v->b,ZombieObjectB) == 0);
    assert(strcmp(v->e,ZombieObjectE) == 0);
    assert(strcmp((char *) p + v->size,ZombieObjectEnd) == 0);
    free(v);
}


#else
#include <execinfo.h>
#endif

#ifdef KER_ZOMBIES

namespace kk {

    struct ZombieObject {
        kk::Object * object;
        Boolean dealloc;
        char** backtrace_symbols;
        int backtrace_size;
        struct ZombieObject * next;
    };
    
}

namespace kk {

    static kk::CString OSZombiesString(int s) {
        switch (s) {
            case SIGSEGV:
                return "SIGSEGV";
            case SIGABRT:
                return "SIGABRT";
            default:
                break;
        }
        return "SIGNAL";
    }
    static void OSZombiesS(int s) {
        
        void * array[10];
        
        int n = backtrace(array,10);
        
        char ** v = backtrace_symbols(array,n);
        
        for(int i=0;i<n;i++) {
            
            if(v[i] != nullptr) {
                kk::Log("[Zombies] [%s] + %s", OSZombiesString(s), v[i]);
            }
            
        }

        Zombies * z = Zombies::current();

        if(z) {
            z->dump();
        }
        
    }
    
    class OSZombies : public Zombies {
    public:
        
        OSZombies():_p(nullptr) {
            pthread_mutex_init(&_lock, nullptr);
            signal(SIGSEGV, OSZombiesS);
            signal(SIGABRT, OSZombiesS);
        }
        
        virtual ~OSZombies() {
            pthread_mutex_destroy(&_lock);
        }
        
        virtual void alloc(Object * object) {
            kk::ZombieObject * v = (kk::ZombieObject *) malloc(sizeof(kk::ZombieObject));
            v->object = object;
            v->dealloc = false;
            
            void * array[10];
            
            v->backtrace_size = backtrace(array,10);
            v->backtrace_symbols = backtrace_symbols(array,v->backtrace_size);
            v->next = nullptr;
            
            pthread_mutex_lock(&_lock);
            
            if(_p == nullptr) {
                _p = v;
            } else {
                v->next = _p;
                _p = v;
            }
            
            pthread_mutex_unlock(&_lock);
            
            
            kk::Log("[Zombies] [alloc] ID:0x%x TH:0x%x",(long) object,pthread_self());
        }
        
        virtual void dealloc(Object * object) {
            
            pthread_mutex_lock(&_lock);
            
            kk::ZombieObject * v = _p;
            
            while(v) {
                if(v->object == object) {
                    break;
                }
                v = v->next;
            }
            
            assert(v != nullptr && !v->dealloc);
            
            v->dealloc = true;
            
            pthread_mutex_unlock(&_lock);
            
            kk::Log("[Zombies] [dealloc] ID:0x%x TH:0x%x",(long) object,pthread_self());
        }
        
        virtual void retain(Object * object) {
            kk::Log("[Zombies] [retain] ID:0x%x TH:0x%x",(long) object,pthread_self());
        }
        
        virtual void release(Object * object) {
            kk::Log("[Zombies] [release] ID:0x%x TH:0x%x",(long) object,pthread_self());
        }
        
        virtual void weak(Object * object, Object ** ptr) {
            kk::Log("[Zombies] [weak] ID:0x%x PTR:0x%x TH:0x%x",(long) object,(long) ptr,pthread_self());
        }
        
        virtual void unWeak(Object * object, Object ** ptr) {
            kk::Log("[Zombies] [unWeak] ID:0x%x PTR:0x%x TH:0x%x",(long) object,(long) ptr,pthread_self());
        }
        
        virtual void dump() {
            
            pthread_mutex_lock(&_lock);
            
            kk::ZombieObject * v = _p;
            
            while(v != nullptr) {
                if(! v->dealloc) {
                    kk::Log("[Zombies] [Object] 0x%x retainCount:%d",(long) (v->object),v->object->retainCount());
                    for(int i=0;i<v->backtrace_size;i++) {
                        if(v->backtrace_symbols[i] == nullptr) {
                            break;
                        }
                        kk::Log("[Zombies] [Object] + %s",v->backtrace_symbols[i]);
                    }
                }
                v = v->next;
            }
            
            pthread_mutex_unlock(&_lock);
            
        }
        
    protected:
        pthread_mutex_t _lock;
        kk::ZombieObject * _p;
    };
    
    Zombies * Zombies::current() {
        static OSZombies v;
        return &v;
    }
    
    
}

#endif
