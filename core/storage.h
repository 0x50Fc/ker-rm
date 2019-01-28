//
//  storage.h
//  Ker
//
//  Created by zhanghailong on 2019/1/7.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#ifndef kk_storage_h
#define kk_storage_h

#include <core/sqlite.h>
#include <core/dispatch.h>

namespace kk {
    
    class Storage : public Object {
    public:
        virtual kk::String get(kk::CString key) = 0;
        virtual kk::Strong<kk::Array<kk::String>> keys() = 0;
        virtual void set(kk::CString key,kk::CString value) = 0;
        virtual void clear() = 0;
        virtual void load(kk::CString key,JSObject * callback);
        virtual void loadKeys(JSObject * callback);
        static void Openlib();
        Ker_CLASS(Storage,Object,"Storage")
    };
    
    class MemStorage : public Storage {
    public:
        virtual kk::Strong<kk::Array<kk::String>> keys();
        virtual kk::String get(kk::CString key);
        virtual void set(kk::CString key,kk::CString value);
        virtual void clear();
    protected:
        std::map<kk::String,kk::String> _data;
    };
    
    class SqliteStorage : public Storage {
    public:
        SqliteStorage(Sqlite * db,kk::DispatchQueue * queue,kk::CString name);
        SqliteStorage(Sqlite * db,kk::CString name);
        virtual kk::String get(kk::CString key);
        virtual void set(kk::CString key,kk::CString value);
        virtual void load(kk::CString key,JSObject * callback);
        virtual void clear();
        virtual kk::Strong<kk::Array<kk::String>> keys();
        virtual void loadKeys(JSObject * callback);
        
        static void install(Sqlite * db);
        
    protected:
        static void get(Sqlite * db,kk::CString name,kk::CString key,kk::String & v);
        static void set(Sqlite * db,kk::CString name,kk::CString key,kk::CString v);
        static void clear(Sqlite * db,kk::CString name);
        static void keys(Sqlite * db,kk::CString name,kk::Array<kk::String> * keys);
        kk::String _name;
        kk::Strong<DispatchQueue> _queue;
        kk::Strong<Sqlite> _db;
    };
    
}

#endif /* storage_h */
