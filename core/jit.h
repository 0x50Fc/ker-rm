//
//  jit.h
//  kk
//
//  Created by zhanghailong on 2018/10/24.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef kk_jit_h
#define kk_jit_h

#include <core/kk.h>
#include <duktape.h>
#include <tuple>
#include <list>
#include <set>
#include <core/dispatch.h>

namespace kk {
    
    
    class JITContext : public Object {
    public:
        JITContext();
        virtual ~JITContext();
        virtual void strong(duk_context * ctx, void * heapptr, Object * object);
        virtual void weak(duk_context * ctx, void * heapptr, Object * object);
        virtual void remove(void * heapptr);
        virtual void remove(Object * object);
        virtual void remove(duk_context * ctx);
        virtual Object * get(void * heapptr);
        virtual void * get(Object * object,duk_context * ctx);
        virtual void forEach(Object * object,std::function<void(duk_context * ,void *)> && func);
        static JITContext * current();
    protected:
        std::map<void *,Weak<Object>> _weakObjects;
        std::map<void *,Strong<Object>> _strongObjects;
        std::map<Object *, std::map<duk_context *,void *>> _objectsWithObject;
    };
    
    template<class ... TArgs>
    class Openlib {
    protected:
        static std::list<std::function<void(duk_context *,TArgs ... args)>> & funcs() {
            static std::list<std::function<void(duk_context *,TArgs ...)>> vs;
            return vs;
        }
    public:
        static void add(std::function<void(duk_context *,TArgs ... args)> && func) {
            funcs().push_back(func);
        }
        static void openlib(duk_context * ctx,TArgs ... args) {
            auto m = funcs();
            auto i = m.begin();
            while(i != m.end()) {
                auto fn = * i;
                fn(ctx, args...);
                i ++;
            }
        }
    };
    
    void OpenBaselib();
    
    class JSResource {
    public:
        virtual String getTextContent(kk::CString path) = 0;
        virtual String getResourceKey(kk::CString path) = 0;
    };
    
    void duk_push_require(duk_context * ctx,kk::CString basePath,JSResource * res);
    
    duk_ret_t duk_require(duk_context * ctx,kk::CString path,JSResource * res);
    
    void addOpenlib(std::function<void(duk_context *)> && func);
    
    void SetPrototype(duk_context * ctx, duk_idx_t idx, const Class * isa);
    
    void SetPrototype(duk_context * ctx, duk_idx_t idx, kk::CString name);
    
    kk::Boolean HasPrototype(duk_context * ctx, kk::CString name);
    
    void SetObject(duk_context * ctx, duk_idx_t idx, Object * object);
    
    void SetWeakObject(duk_context * ctx, duk_idx_t idx, Object * object);
    
    extern void PushNative(duk_context * ctx, kk::Native * native);
    
    void PushObject(duk_context * ctx, Object * object);
    
    void PushWeakObject(duk_context * ctx, Object * object);
    
    Object * GetObject(duk_context * ctx, duk_idx_t idx);
    
    void PushAny(duk_context * ctx, Any & any);
    
    void GetAny(duk_context * ctx, duk_idx_t idx,Any & v);
    
    template<class T>
    Strong<T> Get(duk_context * ctx, duk_idx_t idx) {
        Any v;
        GetAny(ctx, idx, v);
        return (T *) v;
    }
    
    std::shared_ptr<Any> GetAnyPtr(duk_context * ctx, duk_idx_t idx);
    
    void Error(duk_context * ctx, duk_idx_t idx, kk::CString prefix);
    
    namespace details {
        
        template<size_t N>
        struct Arguments {
            
            template<typename P, typename ... TArgs>
            static std::tuple<P,TArgs...> Get(duk_context * ctx,std::vector<std::shared_ptr<Any>> &&vs) {
                std::shared_ptr<Any> v = GetAnyPtr(ctx, - (duk_idx_t) N );
                vs.push_back(v);
                return std::tuple_cat(std::make_tuple((P) *v), Arguments<N-1>::template Get<TArgs...>(ctx,std::move(vs)));
            }
            
            template<typename P, typename ... TArgs>
            static void Set(duk_context * ctx, P v, TArgs ... args) {
                Any a(v);
                PushAny(ctx, a);
                Arguments<N-1>::Set(ctx,args...);
            }
            
        };
        
        template<>
        struct Arguments<1> {
            
            template<typename P>
            static std::tuple<P> Get(duk_context * ctx,std::vector<std::shared_ptr<Any>> &&vs) {
                std::shared_ptr<Any> v = GetAnyPtr(ctx, - 1);
                vs.push_back(v);
                return std::make_tuple((P) *v);
            }
            
            template<typename P>
            static void Set(duk_context * ctx, P v) {
                Any a(v);
                PushAny(ctx, a);
            }
            
        };
        
        template<>
        struct Arguments<0> {
            
            template<typename ... TArgs>
            static std::tuple<> Get(duk_context * ctx,std::vector<std::shared_ptr<Any>> &&vs) {
                return std::tuple<>();
            }
            
            template<typename ... TArgs>
            static void Set(duk_context * ctx) {
            }
            
        };
        
        
        
        template<size_t N>
        struct Call {
            
            template<typename ... TArgs>
            struct T {
                template<typename TReturn,typename ... P>
                static TReturn Apply(std::function<TReturn(P...)> && func,std::tuple<P...>& a,TArgs ... args,typename std::enable_if<std::is_void<TReturn>::value>::type* = 0) {
                    Call<N-1>::template T<typename std::tuple_element<N -1, std::tuple<P...>>::type,TArgs...>::template Apply<TReturn,P ...>(std::move(func),a,std::get<N -1>(a),args...);
                }
                template<typename TReturn,typename ... P>
                static TReturn Apply(std::function<TReturn(P...)> && func,std::tuple<P...>& a,TArgs ... args,typename std::enable_if<!std::is_void<TReturn>::value>::type* = 0) {
                    return Call<N-1>::template T<typename std::tuple_element<N -1, std::tuple<P...>>::type,TArgs...>::template Apply<TReturn,P ...>(std::move(func),a,std::get<N -1>(a),args...);
                }
                
                template<class TClass,typename ... P>
                static TClass * New(std::tuple<P...>& a,TArgs ... args) {
                    return Call<N-1>::template T<typename std::tuple_element<N -1, std::tuple<P...>>::type,TArgs...>::template New<TClass,P ...>(a,std::get<N -1>(a),args...);
                }
            };
        };
        
        template<>
        struct Call<0> {
            
            template<typename ... TArgs>
            struct T {
                template<typename TReturn,typename ... P>
                static TReturn Apply(std::function<TReturn(P...)> && func,std::tuple<P...>& a, P ... args,typename std::enable_if<std::is_void<TReturn>::value>::type* = 0) {
                    func(args...);
                }
                template<typename TReturn,typename ... P>
                static TReturn Apply(std::function<TReturn(P...)> && func,std::tuple<P...>& a, P ... args,typename std::enable_if<!std::is_void<TReturn>::value>::type* = 0) {
                    return func(args...);
                }
                
                template<class TClass,typename ... P>
                static TClass * New(std::tuple<P...>& a, P ... args) {
                    return new TClass(args...);
                }
            };
            
        };
        
    }
    
    template <typename TReturn,typename ... TArgs>
    duk_ret_t Call(std::function<TReturn(TArgs...)> && func,duk_context * ctx,typename std::enable_if<std::is_void<TReturn>::value>::type* = 0) {
        std::vector<std::shared_ptr<Any>> vs;
        std::tuple<TArgs...> args = details::Arguments<sizeof...(TArgs)>::template Get<TArgs...>(ctx, std::move(vs));
        try {
            details::Call<sizeof...(TArgs)>::template T<>::template Apply<TReturn,TArgs...>(std::move(func),args);
        }
        catch(kk::CString errmsg){
            duk_push_error_object(ctx, DUK_ERR_ERROR, "%s", errmsg);
            return duk_throw(ctx);
        }
        return 0;
    }
    
    template <typename TReturn,typename ... TArgs>
    duk_ret_t Call(std::function<TReturn(TArgs...)> && func,duk_context * ctx,typename std::enable_if<!std::is_void<TReturn>::value>::type* = 0) {
        std::vector<std::shared_ptr<Any>> vs;
        std::tuple<TArgs...> args = details::Arguments<sizeof...(TArgs)>::template Get<TArgs...>(ctx, std::move(vs));
        try {
            Any v = details::Call<sizeof...(TArgs)>::template T<>::template Apply<TReturn,TArgs...>(std::move(func),args);
            PushAny(ctx, v);
        }
        catch(kk::CString errmsg){
            duk_push_error_object(ctx, DUK_ERR_ERROR, "%s", errmsg);
            return duk_throw(ctx);
        }
        return 1;
    }
    
    template <class T,typename ... TArgs>
    T * New(duk_context * ctx) {
        std::vector<std::shared_ptr<Any>> vs;
        std::tuple<TArgs...> args = details::Arguments<sizeof...(TArgs)>::template Get<TArgs...>(ctx, std::move(vs));
        return details::Call<sizeof...(TArgs)>::template T<>::template New<T,TArgs...>(args);
    }

    template<typename TReturn,typename ... TArgs>
    void PushFunction(duk_context * ctx, TFunction<TReturn,TArgs...> * func) {
        auto fn = [](duk_context * ctx) -> duk_ret_t {
            
            duk_push_current_function(ctx);
            TFunction<TReturn,TArgs...> * func = (TFunction<TReturn,TArgs...> *) GetObject(ctx,  -1);
            duk_pop(ctx);
            
            if(func) {
                std::function<TReturn(TArgs...)> & fn = * func;
                return Call(std::move(fn),ctx);
            }
           
            return 0;
        };
        duk_push_c_function(ctx, fn, sizeof...(TArgs));
        SetObject(ctx, -1, func);
    }
    
    template<typename TReturn,typename ... TArgs>
    void PushFunction(duk_context * ctx, TReturn (*func) (TArgs ...)) {
        auto fn = [](duk_context * ctx) -> duk_ret_t {
            duk_push_current_function(ctx);
            duk_get_prop_string(ctx, -1, "__func");
            TReturn(*func)(TArgs ...) = (TReturn(*)(TArgs ...)) duk_to_pointer(ctx, -1);
            duk_pop_2(ctx);
            
            if(func) {
                std::function<TReturn(TArgs...)> v(func);
                return Call(std::move(v),ctx);
            }
            return 0;
        };
        duk_push_c_function(ctx, fn, sizeof...(TArgs));
        {
            duk_push_pointer(ctx, (void *) func);
            duk_put_prop_string(ctx, -2, "__func");
        }
    }
    
    template<typename T,typename ... TArgs>
    void PushStrongFunction(duk_context * ctx, kk::Strong<T> (*func) (TArgs ...)) {
        auto fn = [](duk_context * ctx) -> duk_ret_t {
            duk_push_current_function(ctx);
            duk_get_prop_string(ctx, -1, "__func");
            kk::Strong<T>(*func)(TArgs ...) = (kk::Strong<T>(*)(TArgs ...)) duk_to_pointer(ctx, -1);
            duk_pop_2(ctx);
            
            if(func) {
                std::function<kk::Any(TArgs...)> v([func](TArgs ... args) -> kk::Any{
                    kk::Strong<T> v = (*func)(args...);
                    return Any(v.get());
                });
                return Call(std::move(v),ctx);
            }
            return 0;
        };
        duk_push_c_function(ctx, fn, sizeof...(TArgs));
        {
            duk_push_pointer(ctx, (void *) func);
            duk_put_prop_string(ctx, -2, "__func");
        }
    }
    
    template<class TObject,typename T,typename ... TArgs>
    struct Method {
        T (TObject::*method)(TArgs ...args);
    };
    
    template<class TObject,typename T>
    void PutProperty(duk_context * ctx, duk_idx_t idx, CString name, T (TObject::*getter)(), void (TObject::*setter)(T value)) {

        auto getter_fn = [](duk_context * ctx) -> duk_ret_t {
            duk_push_this(ctx);
            TObject * object = (TObject *) GetObject(ctx, -1);
            duk_pop(ctx);
            
            Method<TObject,T> * GETTER = nullptr;
     
            duk_push_current_function(ctx);
            duk_get_prop_string(ctx, -1, "__func");
            GETTER = (Method<TObject,T> *) duk_get_buffer(ctx, -1, nullptr);
            duk_pop_2(ctx);
            if(object && GETTER && GETTER->method != nullptr) {
                Any v = (object->*GETTER->method)();
                PushAny(ctx, v);
                return 1;
            }
            return 0;
        };
        
        auto setter_fn = [](duk_context * ctx) -> duk_ret_t {
            duk_push_this(ctx);
            TObject * object = (TObject *) GetObject(ctx, -1);
            duk_pop(ctx);
            
            Method<TObject,void,T> * SETTER = nullptr;
            
            duk_push_current_function(ctx);
            duk_get_prop_string(ctx, -1, "__func");
            SETTER = (Method<TObject,void,T> *) duk_get_buffer(ctx, -1, nullptr);
            duk_pop_2(ctx);
            
            if(object && SETTER && SETTER->method != nullptr) {
                Any v ;
                GetAny(ctx,-1,v);
                (object->*SETTER->method)((T) v);
            }
            return 0;
        };
        
        
        Method<TObject,T> GETTER;
        Method<TObject, void,T> SETTER;
        
        GETTER.method = getter;
        SETTER.method = setter;
        
        duk_push_string(ctx, name);
        duk_push_c_function(ctx, getter_fn, 0);
        {
            void * v = duk_push_fixed_buffer(ctx, sizeof(GETTER));
            memcpy(v,&GETTER,sizeof(GETTER));
            duk_put_prop_string(ctx, -2, "__func");
        }
    
        duk_push_c_function(ctx, setter_fn, 1);
        {
            void * v = duk_push_fixed_buffer(ctx, sizeof(SETTER));
            memcpy(v,&SETTER,sizeof(SETTER));
            duk_put_prop_string(ctx, -2, "__func");
        }
        
        duk_def_prop(ctx, idx - 3, DUK_DEFPROP_HAVE_GETTER | DUK_DEFPROP_HAVE_SETTER | DUK_DEFPROP_SET_ENUMERABLE | DUK_DEFPROP_SET_CONFIGURABLE);
        
    }
    
    template<class TObject,typename T>
    void PutProperty(duk_context * ctx, duk_idx_t idx, CString name, T (TObject::*getter)()) {
        
        auto getter_fn = [](duk_context * ctx) -> duk_ret_t {
            duk_push_this(ctx);
            TObject * object = (TObject *) GetObject(ctx, -1);
            duk_pop(ctx);
            
            Method<TObject,T> * GETTER = nullptr;
            
            duk_push_current_function(ctx);
            duk_get_prop_string(ctx, -1, "__func");
            GETTER = (Method<TObject,T> *) duk_get_buffer(ctx, -1, nullptr);
            duk_pop_2(ctx);
            if(object && GETTER && GETTER->method != nullptr) {
                Any v = (object->*GETTER->method)();
                PushAny(ctx, v);
                return 1;
            }
            return 0;
        };
        
        
        Method<TObject,T> GETTER;
        
        GETTER.method = getter;
        
        duk_push_string(ctx, name);
        duk_push_c_function(ctx, getter_fn, 0);
        {
            void * v = duk_push_fixed_buffer(ctx, sizeof(GETTER));
            memcpy(v,&GETTER,sizeof(GETTER));
            duk_put_prop_string(ctx, -2, "__func");
        }
        
        duk_def_prop(ctx, idx - 2, DUK_DEFPROP_HAVE_GETTER | DUK_DEFPROP_SET_ENUMERABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    }
    
    template<class TObject,typename T>
    void PutStrongProperty(duk_context * ctx, duk_idx_t idx, CString name, kk::Strong<T> (TObject::*getter)()) {
        
        auto getter_fn = [](duk_context * ctx) -> duk_ret_t {
            duk_push_this(ctx);
            TObject * object = (TObject *) GetObject(ctx, -1);
            duk_pop(ctx);
            
            Method<TObject,kk::Strong<T>> * GETTER = nullptr;
            
            duk_push_current_function(ctx);
            duk_get_prop_string(ctx, -1, "__func");
            GETTER = (Method<TObject,kk::Strong<T>> *) duk_get_buffer(ctx, -1, nullptr);
            duk_pop_2(ctx);
            if(object && GETTER && GETTER->method != nullptr) {
                kk::Strong<T> strong = (object->*GETTER->method)();
                Any v = strong.get();
                PushAny(ctx, v);
                return 1;
            }
            return 0;
        };
        
        
        Method<TObject,kk::Strong<T>> GETTER;
        
        GETTER.method = getter;
        
        duk_push_string(ctx, name);
        duk_push_c_function(ctx, getter_fn, 0);
        {
            void * v = duk_push_fixed_buffer(ctx, sizeof(GETTER));
            memcpy(v,&GETTER,sizeof(GETTER));
            duk_put_prop_string(ctx, -2, "__func");
        }
        
        duk_def_prop(ctx, idx - 2, DUK_DEFPROP_HAVE_GETTER | DUK_DEFPROP_SET_ENUMERABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    }
    
    template<class TObject,typename TReturn,typename ... TArgs>
    void PutMethod(duk_context * ctx, duk_idx_t idx, CString name, TReturn (TObject::*method)(TArgs ...),typename std::enable_if<std::is_void<TReturn>::value>::type* = 0) {
    
        auto fn = [](duk_context * ctx) -> duk_ret_t {
            duk_push_this(ctx);
            TObject * object = (TObject *) GetObject(ctx, -1);
            duk_pop(ctx);
            
            Method<TObject,TReturn,TArgs...> * METHOD = nullptr;
            
            duk_push_current_function(ctx);
            duk_get_prop_string(ctx, -1, "__func");
            METHOD = (Method<TObject,TReturn,TArgs...> *) duk_get_buffer(ctx, -1, nullptr);
            duk_pop_2(ctx);
 
            if(object && METHOD && METHOD->method != nullptr) {
                std::function<TReturn(TArgs...)> fn = [object,METHOD](TArgs ... args) -> TReturn {
                    (object->*METHOD->method)(args...);
                };
                return Call(std::move(fn), ctx);
            }
            return 0;
        };
        
        Method<TObject,TReturn,TArgs...> METHOD;
        
        METHOD.method = method;
        
        duk_push_string(ctx, name);
        duk_push_c_function(ctx, fn, sizeof...(TArgs));
        {
            void * v = duk_push_fixed_buffer(ctx, sizeof(METHOD));
            memcpy(v,&METHOD,sizeof(METHOD));
            duk_put_prop_string(ctx, -2, "__func");
        }
        duk_def_prop(ctx, idx - 2, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_ENUMERABLE | DUK_DEFPROP_SET_CONFIGURABLE | DUK_DEFPROP_CLEAR_WRITABLE);
        
    }
    
    template<class TObject,typename TReturn,typename ... TArgs>
    void PutMethod(duk_context * ctx, duk_idx_t idx, CString name, TReturn (TObject::*method)(TArgs ...),typename std::enable_if<!std::is_void<TReturn>::value>::type* = 0) {
        
        auto fn = [](duk_context * ctx) -> duk_ret_t {
            duk_push_this(ctx);
            TObject * object = (TObject *) GetObject(ctx, -1);
            duk_pop(ctx);
            
            Method<TObject,TReturn,TArgs...> * METHOD = nullptr;
            
            duk_push_current_function(ctx);
            duk_get_prop_string(ctx, -1, "__func");
            METHOD = (Method<TObject,TReturn,TArgs...> *) duk_get_buffer(ctx, -1, nullptr);
            duk_pop_2(ctx);
            
            if(object && METHOD && METHOD->method != nullptr) {
                std::function<TReturn(TArgs...)> fn = [object,METHOD](TArgs ... args) -> TReturn {
                    return (object->*METHOD->method)(args...);
                };
                return Call(std::move(fn), ctx);
            }
            return 0;
        };
        
        Method<TObject,TReturn,TArgs...> METHOD;
        
        METHOD.method = method;
        
        duk_push_string(ctx, name);
        duk_push_c_function(ctx, fn, sizeof...(TArgs));
        {
            void * v = duk_push_fixed_buffer(ctx, sizeof(METHOD));
            memcpy(v,&METHOD,sizeof(METHOD));
            duk_put_prop_string(ctx, -2, "__func");
        }
        duk_def_prop(ctx, idx - 2, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_ENUMERABLE | DUK_DEFPROP_SET_CONFIGURABLE | DUK_DEFPROP_CLEAR_WRITABLE);
        
    }
    
    template<class TObject,class T,typename ... TArgs>
    void PutStrongMethod(duk_context * ctx, duk_idx_t idx, CString name, kk::Strong<T> (TObject::*method)(TArgs ...)) {
        
        auto fn = [](duk_context * ctx) -> duk_ret_t {
            duk_push_this(ctx);
            TObject * object = (TObject *) GetObject(ctx, -1);
            duk_pop(ctx);
            
            Method<TObject,kk::Strong<T>,TArgs...> * METHOD = nullptr;
            
            duk_push_current_function(ctx);
            duk_get_prop_string(ctx, -1, "__func");
            METHOD = (Method<TObject,kk::Strong<T>,TArgs...> *) duk_get_buffer(ctx, -1, nullptr);
            duk_pop_2(ctx);
            
            if(object && METHOD && METHOD->method != nullptr) {
                std::function<kk::Any(TArgs...)> fn = [object,METHOD](TArgs ... args) -> kk::Any {
                    kk::Strong<T> v = (object->*METHOD->method)(args...);
                    return kk::Any(v.get());
                };
                return Call(std::move(fn), ctx);
            }
            return 0;
        };
        
        Method<TObject,kk::Strong<T>,TArgs...> METHOD;
        
        METHOD.method = method;
        
        duk_push_string(ctx, name);
        duk_push_c_function(ctx, fn, sizeof...(TArgs));
        {
            void * v = duk_push_fixed_buffer(ctx, sizeof(METHOD));
            memcpy(v,&METHOD,sizeof(METHOD));
            duk_put_prop_string(ctx, -2, "__func");
        }
        duk_def_prop(ctx, idx - 2, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_ENUMERABLE | DUK_DEFPROP_SET_CONFIGURABLE | DUK_DEFPROP_CLEAR_WRITABLE);
        
    }
    
    template<class TObject,typename TReturn,typename ... TArgs>
    void PutMethod(duk_context * ctx, duk_idx_t idx, CString name, TReturn (*func)(TObject * object,TArgs ...),typename std::enable_if<std::is_void<TReturn>::value>::type* = 0) {
        
        auto fn = [](duk_context * ctx) -> duk_ret_t {
            duk_push_this(ctx);
            TObject * object = (TObject *) GetObject(ctx, -1);
            duk_pop(ctx);
            
            Method<TObject,TReturn,TArgs...> * METHOD = nullptr;
            
            duk_push_current_function(ctx);
            duk_get_prop_string(ctx, -1, "__func");
            TReturn (*func)(TObject * object,TArgs ...) = (TReturn (*)(TObject * object,TArgs ...)) duk_to_pointer(ctx, -1);
            duk_pop_2(ctx);
            
            if(object && METHOD && METHOD->method != nullptr) {
                std::function<TReturn(TObject,TArgs...)> fn = [object,func](TArgs ... args) -> TReturn {
                    (*func)(object,args...);
                };
                return Call(std::move(fn), ctx);
            }
            return 0;
        };
        
        duk_push_string(ctx, name);
        duk_push_c_function(ctx, fn, sizeof...(TArgs));
        {
            duk_push_pointer(ctx, func);
            duk_put_prop_string(ctx, -2, "__func");
        }
        duk_def_prop(ctx, idx - 2, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_ENUMERABLE | DUK_DEFPROP_SET_CONFIGURABLE | DUK_DEFPROP_CLEAR_WRITABLE);
        
    }
    
    template<class TObject,typename TReturn,typename ... TArgs>
    void PutMethod(duk_context * ctx, duk_idx_t idx, CString name, TReturn (*func)(TObject * object,TArgs ...),typename std::enable_if<!std::is_void<TReturn>::value>::type* = 0) {
        
        auto fn = [](duk_context * ctx) -> duk_ret_t {
            duk_push_this(ctx);
            TObject * object = (TObject *) GetObject(ctx, -1);
            duk_pop(ctx);
            
            Method<TObject,TReturn,TArgs...> * METHOD = nullptr;
            
            duk_push_current_function(ctx);
            duk_get_prop_string(ctx, -1, "__func");
            TReturn (*func)(TObject * object,TArgs ...) = (TReturn (*)(TObject * object,TArgs ...)) duk_to_pointer(ctx, -1);
            duk_pop_2(ctx);
            
            if(object && METHOD && METHOD->method != nullptr) {
                std::function<TReturn(TObject,TArgs...)> fn = [object,func](TArgs ... args) -> TReturn {
                    return (*func)(object,args...);
                };
                return Call(std::move(fn), ctx);
            }
            return 0;
        };
        
        duk_push_string(ctx, name);
        duk_push_c_function(ctx, fn, sizeof...(TArgs));
        {
            duk_push_pointer(ctx, func);
            duk_put_prop_string(ctx, -2, "__func");
        }
        duk_def_prop(ctx, idx - 2, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_ENUMERABLE | DUK_DEFPROP_SET_CONFIGURABLE | DUK_DEFPROP_CLEAR_WRITABLE);
        
    }
    
    template<class T,typename ... TArgs>
    void PushConstructor(duk_context * ctx) {
        
        auto fn =  [](duk_context * ctx) -> duk_ret_t {
            
            kk::Strong<T> v(New<T,TArgs...>(ctx));
            
            duk_push_this(ctx);
            
            SetObject(ctx, -1, v.get());
            
            duk_push_current_function(ctx);
            duk_get_prototype(ctx, -1);
            
            duk_remove(ctx, -2);
            duk_set_prototype(ctx, -2);
            
            duk_pop(ctx);
            
            return 0;
        };
        
        duk_push_c_function(ctx, fn, sizeof...(TArgs));
        
    }
    
    template<class T,typename ... TArgs>
    void PushClass(duk_context * ctx, std::function<void(duk_context *)> && func) {
        
        const Class * isa = T::Class();
        
        PushConstructor<T,TArgs...>(ctx);
        
        duk_push_object(ctx);
        
        if(isa->isa) {
            SetPrototype(ctx, -1, isa->isa);
        }
        
        if(func != nullptr) {
            func(ctx);
        }
        
        duk_set_prototype(ctx, -2);
        
        duk_put_global_string(ctx, isa->name);
    }
    
    
    
    template<class T>
    void PushInterface(duk_context * ctx, std::function<void(duk_context *)> && func) {
        
        const Class * isa = T::Class();
        
        duk_push_c_function(ctx, [](duk_context * ctx)->duk_ret_t{
            return 0;
        }, 0);
        
        duk_push_object(ctx);
        
        if(isa->isa) {
            SetPrototype(ctx, -1, isa->isa);
        }
        
        if(func != nullptr) {
            func(ctx);
        }
        
        duk_set_prototype(ctx, -2);
        
        duk_put_global_string(ctx, isa->name);
    }
   
    class _JSFunction : public Function {
    public:
        virtual duk_c_function getInvoke() = 0;
        virtual duk_int_t getArguments() = 0;
    };
    
    template<typename T,typename ... TArgs>
    class JSFunction : public _JSFunction {
    public:
        JSFunction(std::function<T(TArgs ...)> && func):_func(func){}
        
        virtual duk_c_function getInvoke() {
            return JSFunction<T,TArgs...>::invoke;
        }
        
        virtual duk_int_t getArguments() {
            return sizeof...(TArgs);
        }
        
        static duk_ret_t invoke(duk_context * ctx) {
            duk_push_current_function(ctx);
            JSFunction<T,TArgs...> * func = (JSFunction<T,TArgs...> *) GetObject(ctx,  -1);
            duk_pop(ctx);
            if(func) {
                std::function<T(TArgs...)> & fn = * func;
                return Call(std::move(fn),ctx);
            }
            return 0;
        }
        
        T operator()(TArgs ... args) {
            return _func(args...);
        }
        
        operator std::function<T(TArgs ...)>&() {
            return _func;
        }
    private:
        std::function<T(TArgs ...)> _func;
    };
    
    class JSRecycle {
    public:
        virtual void recycle(duk_context * ctx) = 0;
    };
    
    class JSObject : public Object , public Copying, public Getter , public JSRecycle {
    public:
        JSObject(duk_context * ctx, void * heapptr);
        virtual ~JSObject();
        virtual void recycle(duk_context * ctx);
        virtual duk_context * jsContext();
        virtual void * heapptr();
        virtual DispatchQueue * queue();
        virtual kk::Strong<Object> copy();
        virtual void get(kk::CString key,Any & value);
        template<typename T,typename ... TArgs>
        T invoke(JSObject * object,TArgs ... args,typename std::enable_if<std::is_void<T>::value>::type* = 0) {
            
            duk_context * ctx = this->jsContext();
            void * heapptr = this->heapptr();
            
            if(ctx && heapptr) {
                
                duk_push_heapptr(ctx, heapptr);
                
                if(duk_is_function(ctx, -1)) {
                    
                    if(object != nullptr && object->jsContext() == ctx) {
                        duk_push_heapptr(ctx, object->heapptr());
                    }
                    
                    details::Arguments<sizeof...(TArgs)>::template Set(ctx,args...);
                    
                    if(object != nullptr && object->jsContext() == ctx) {
                        if(duk_pcall_method(ctx, sizeof...(TArgs)) != DUK_EXEC_SUCCESS) {
                            Error(ctx, -1, "[JSObject]");
                        }
                    } else {
                        if(duk_pcall(ctx, sizeof...(TArgs)) != DUK_EXEC_SUCCESS) {
                            Error(ctx, -1, "[JSObject]");
                        }
                    }
                }
                
                duk_pop(ctx);
                
            }
        }
        
        template<typename T,typename ... TArgs>
        T invoke(JSObject * object,TArgs ... args,typename std::enable_if<!std::is_void<T>::value>::type* = 0) {
            
            Any r;
            
            duk_context * ctx = this->jsContext();
            void * heapptr = this->heapptr();
            
            if(ctx && heapptr) {
                
                duk_push_heapptr(ctx, heapptr);
                
                if(duk_is_function(ctx, -1)) {
                    
                    if(object != nullptr && object->jsContext() == ctx) {
                        duk_push_heapptr(ctx, object->heapptr());
                    }
                    
                    details::Arguments<sizeof...(TArgs)>::template Set(ctx,args...);
                    
                    if(object != nullptr && object->jsContext() == ctx) {
                        if(duk_pcall_method(ctx, sizeof...(TArgs)) == DUK_EXEC_SUCCESS) {
                            GetAny(ctx, -1, r);
                        } else {
                            Error(ctx, -1, "[JSObject]");
                        }
                    } else {
                        if(duk_pcall(ctx, sizeof...(TArgs)) == DUK_EXEC_SUCCESS) {
                            GetAny(ctx, -1, r);
                        } else {
                            Error(ctx, -1, "[JSObject]");
                        }
                    }
                }
                
                duk_pop(ctx);
                
            }
            
            return (T) r;
        }
        
        operator kk::Strong<TObject<kk::String,kk::Any>>();
        operator kk::Strong<Array<kk::Any>>();
        
    protected:
        duk_context * _ctx;
        kk::Strong<DispatchQueue> _queue;
    };
    
    
    enum SignatureType {
        SignatureTypeVoid,
        SignatureTypeInt8,
        SignatureTypeUint8,
        SignatureTypeInt16,
        SignatureTypeUint16,
        SignatureTypeInt32,
        SignatureTypeUint32,
        SignatureTypeInt64,
        SignatureTypeUint64,
        SignatureTypeFloat32,
        SignatureTypeFloat64,
        SignatureTypeBoolean,
        SignatureTypeObject,
        SignatureTypeCString,
        SignatureTypeNative,
    };
    
    struct Signature;
    
    typedef void (*SignatureFromFunc)(Signature * s,duk_context * ctx, duk_idx_t idx);
    typedef void (*SignatureToFunc)(Signature * s,duk_context * ctx);
    
    struct Signature {
        SignatureType type;
        SignatureFromFunc from;
        SignatureToFunc to;
        union {
            Int8 int8Value;
            Uint8 uint8Value;
            Int16 int16Value;
            Uint16 uint16Value;
            Int32 int32Value;
            Uint32 uint32Value;
            Int64 int64Value;
            Uint64 uint64Value;
            Float32 float32Value;
            Float64 float64Value;
            Boolean booleanValue;
            CString cstringValue;
            kk::Native * nativeValue;
        };
        Strong<kk::Object> objectValue;
    };
    
    void duk_push_Signature(duk_context * ctx,Signature & v);
    
    void duk_get_Signature(duk_context * ctx,duk_idx_t idx,Signature & v);
    
    void duk_get_Arguments(duk_context * ctx,std::vector<Signature> & arguments);
    
    void duk_pcall(duk_context * ctx,Signature returnSignature,std::vector<Signature> & arguments);
    
    void duk_pcall_method(duk_context * ctx,Signature returnSignature,std::vector<Signature> & arguments);
    
    duk_ret_t duk_json_decode(duk_context * ctx,void * data,size_t size);
   
}

#endif /* jit_h */
