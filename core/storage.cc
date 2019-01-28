//
//  storage.cc
//  Ker
//
//  Created by zhanghailong on 2019/1/7.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#include <core/storage.h>

namespace kk {
    
    void Storage::load(kk::CString key,JSObject * callback) {
        if(callback != nullptr) {
            kk::String a = get(key);
            callback->invoke<void,kk::CString>(nullptr, a.c_str());
        }
    }
    
    void Storage::loadKeys(JSObject * callback) {
        if(callback != nullptr) {
            kk::Strong<kk::Array<kk::String>> v = keys();
            callback->invoke<void,kk::Array<kk::String> *>(nullptr, (kk::Array<kk::String> *) v);
        }
    }
    
    void Storage::Openlib() {
        
        kk::Openlib<>::add([](duk_context * ctx)->void{
            
            kk::PushInterface<Storage>(ctx, [](duk_context * ctx)->void{
                
                kk::PutMethod<Storage,kk::String,kk::CString>(ctx, -1, "get", &Storage::get);
                kk::PutMethod<Storage,void,kk::CString,kk::CString>(ctx, -1, "set", &Storage::set);
                kk::PutMethod<Storage,void,kk::CString,JSObject *>(ctx, -1, "load", &Storage::load);
                kk::PutMethod<Storage,void>(ctx, -1, "clear", &Storage::clear);
                kk::PutStrongMethod<Storage,kk::Array<kk::String>>(ctx, -1, "keys", &Storage::keys);
                kk::PutMethod<Storage,void,JSObject *>(ctx, -1, "loadKeys", &Storage::loadKeys);
                
            });
            
        });
        
    }
    
    kk::Strong<kk::Array<kk::String>> MemStorage::keys() {
        kk::Strong<kk::Array<kk::String>> v = new kk::Array<kk::String>();
        auto i = _data.begin();
        auto e = _data.end();
        while(i != e) {
            v->push((kk::String &) i->first);
            i ++;
        }
        return v;
    }
    
    kk::String MemStorage::get(kk::CString key) {
        if(key == nullptr) {
            return nullptr;
        }
        
        auto i = _data.find(key);
        
        if(i != _data.end()){
            return i->second;
        }
        
        return nullptr;
    }
    
    void MemStorage::set(kk::CString key,kk::CString value) {
        
        if(key == nullptr) {
            return;
        }
        
        if(value == nullptr) {
            auto i = _data.find(key);
            if(i != _data.end()) {
                _data.erase(i);
            }
        } else {
            _data[key] = value;
        }
    }
    
    void MemStorage::clear() {
        _data.clear();
    }
    

    SqliteStorage::SqliteStorage(Sqlite * db,kk::DispatchQueue * queue,kk::CString name):_db(db),_queue(queue),_name(name) {
        
        kk::String n(name);
        kk::Strong<Sqlite> v = db;
        
        _queue->async([n,v]()->void{
            try {
                kk::String sql;
                sql.append("CREATE TABLE IF NOT EXISTS [_");
                sql.append(n);
                sql.append("] (key VARCHAR(4096) PRIMARY KEY , value TEXT)");
                v->exec(sql.c_str(), (kk::Array<kk::Any> *) nullptr);
            }
            catch(kk::CString errmsg) {
                kk::Log("[SqliteStorage::SqliteStorage] %s",errmsg);
            }
        });
        
    }
    
    SqliteStorage::SqliteStorage(Sqlite * db,kk::CString name):SqliteStorage(db,IODispatchQueue(),name) {
        
    }
    
    kk::String SqliteStorage::get(kk::CString key) {
        if(key == nullptr) {
            return kk::Any();
        }
        kk::String n(_name);
        kk::String k(key);
        kk::String v;
        kk::Strong<Sqlite> db = _db;
        _queue->sync([n,k,&v,db]()->void{
            SqliteStorage::get(db.operator->(),n.c_str(),k.c_str(),v);
        });
        return v;
    }
    
    void SqliteStorage::set(kk::CString key,kk::CString value) {
        
        if(key == nullptr) {
            return;
        }
        
        kk::String n(_name);
        kk::String k(key);
        kk::String v;
        if(value) {
            v.append(value);
        }
        kk::Strong<Sqlite> db = _db;
        _queue->async([n,k,value,db,v]()->void{
            SqliteStorage::set(db.operator->(),n.c_str(),k.c_str(), v.empty() ? nullptr : v.c_str() );
        });
    }
    
    void SqliteStorage::clear() {
        kk::String n(_name);
        kk::Strong<Sqlite> db = _db;
        _queue->async([n,db]()->void{
            SqliteStorage::clear(db.operator->(),n.c_str());
        });
    }
    
    void SqliteStorage::load(kk::CString key,JSObject * callback) {
        
        if(callback == nullptr) {
            return;
        }
        
        if(key == nullptr) {
            if(callback) {
                callback->invoke<void>(nullptr);
            }
            return;
        }
        
        kk::Strong<DispatchQueue> queue = getCurrentDispatchQueue();
        kk::Strong<JSObject> cb = callback;
        kk::String n(_name);
        kk::String k(key);
        kk::Any v;
        kk::Strong<Sqlite> db = _db;
        _queue->async([n,k,db,cb,queue]()->void{
            kk::String v;
            SqliteStorage::get(db.operator->(),n.c_str(),k.c_str(),v);
            queue->async([v,cb]()->void{
                JSObject * callback = cb.operator->();
                if(callback) {
                    callback->invoke<void,kk::CString>(nullptr, v.c_str());
                }
            });
        });
    }
    
    kk::Strong<kk::Array<kk::String>> SqliteStorage::keys() {
        kk::String n(_name);
        kk::Array<kk::String> * v = new kk::Array<kk::String>();
        kk::Strong<Sqlite> db = _db;
        _queue->sync([n,v,db]()->void{
            SqliteStorage::keys(db.operator->(),n.c_str(),v);
        });
        return v;
    }
    
    void SqliteStorage::loadKeys(JSObject * callback) {
        
        if(callback == nullptr) {
            return;
        }
        
        kk::Strong<DispatchQueue> queue = getCurrentDispatchQueue();
        kk::String n(_name);
        kk::Strong<Sqlite> db = _db;
        kk::Strong<JSObject> cb = callback;
        _queue->sync([n,db,cb,queue]()->void{
            kk::Strong<kk::Array<kk::String>> v = new kk::Array<kk::String>();
            SqliteStorage::keys(db.operator->(),n.c_str(),v);
            queue->async([v,cb]()->void{
                JSObject * callback = cb.operator->();
                if(callback) {
                    callback->invoke<void,kk::Array<kk::String> *>(nullptr, v.operator->());
                }
            });
        });
        
    }
    
    void SqliteStorage::get(Sqlite * db,kk::CString name,kk::CString key,kk::String & v) {
        
        if(key == nullptr) {
            return;
        }
        
        try {
            
            kk::Strong<kk::Array<kk::Any>> data = new kk::Array<kk::Any>({key});
            
            kk::String sql;
            
            sql.append("SELECT value FROM [_").append(name).append("] WHERE key=?");
            
            auto rs = db->query(sql.c_str(), (kk::Array<kk::Any> *) data);
            
            if(rs->next()) {
                kk::Any r = rs->getValue(0);
                kk::CString s = r;
                if(s != nullptr) {
                    v.append(s);
                }
            }
            
            rs->close();
            
        } catch(kk::CString errmsg) {
            kk::Log("[SqliteStorage::set] %s",errmsg);
        }
        
    }
    
    void SqliteStorage::set(Sqlite * db,kk::CString name, kk::CString key,kk::CString v) {
        
        if(key == nullptr) {
            return;
        }
        
        try {
            
            kk::Boolean hasKey = false;
            
            kk::Strong<kk::Array<kk::Any>> data = new kk::Array<kk::Any>({key});
            
            kk::String sql;
            
            sql.append("SELECT COUNT(*) FROM [_").append(name).append("] WHERE key=?");
            
            auto rs = db->query(sql.c_str(), (kk::Array<kk::Any> *) data);
            
            if(rs->next()) {
                kk::Any v = rs->getValue(0);
                hasKey = ((kk::Int32) v) > 0;
            }
        
            rs->close();
            
            if(hasKey) {
                if(v == nullptr) {
                    sql.clear();
                    sql.append("DELETE FROM [_").append(name).append("] WHERE key=?");
                    db->exec(sql.c_str(), (kk::Array<kk::Any> *) data);
                } else {
                    sql.clear();
                    sql.append("UPDATE [_").append(name).append("] SET value=? WHERE key=?");
                    data = new kk::Array<kk::Any>({v,key});
                    db->exec(sql.c_str(), (kk::Array<kk::Any> *) data);
                }
            } else if(v != nullptr) {
                sql.clear();
                sql.append("INSERT INTO [_").append(name).append("](key,value) VALUES(?,?)");
                data = new kk::Array<kk::Any>({key,v});
                db->exec(sql.c_str(), (kk::Array<kk::Any> *) data);
            }
            
        } catch(kk::CString errmsg) {
            kk::Log("[SqliteStorage::set] %s",errmsg);
        }
    }
    
    void SqliteStorage::clear(Sqlite * db,kk::CString name) {
        
        try {
            kk::String sql;
            
            sql.append("DELETE FROM [_").append(name).append("]");
            
            db->exec(sql.c_str(), (kk::Array<kk::Any> * ) nullptr);
            
        } catch(kk::CString errmsg) {
            kk::Log("[SqliteStorage::set] %s",errmsg);
        }
        
    }
    
    void SqliteStorage::keys(Sqlite * db,kk::CString name,kk::Array<kk::String> * keys) {
        
        try {
            
            kk::String sql;
            
            sql.append("SELECT key FROM [_").append(name).append("]");
            
            auto rs = db->query(sql.c_str(), (kk::Array<kk::Any> *) nullptr);
            
            while(rs->next()) {
                kk::Any v = rs->getValue(0);
                kk::CString s = (kk::CString) v;
                if(s) {
                    keys->push(s);
                }
            }
            
            rs->close();
            
        } catch(kk::CString errmsg) {
            kk::Log("[SqliteStorage::set] %s",errmsg);
        }
        
    }
    
}
