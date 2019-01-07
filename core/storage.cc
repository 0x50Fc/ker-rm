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
    

    SqliteStorage::SqliteStorage(Sqlite * db,kk::DispatchQueue * queue,kk::CString prefix):_db(db),_queue(queue),_prefix(prefix) {
        
    }
    
    SqliteStorage::SqliteStorage(Sqlite * db,kk::CString prefix):SqliteStorage(db,IODispatchQueue(),prefix) {
        
    }
    
    kk::String SqliteStorage::get(kk::CString key) {
        if(key == nullptr) {
            return kk::Any();
        }
        kk::String n(_prefix);
        n.append(key);
        kk::String v;
        kk::Strong<Sqlite> db = _db;
        _queue->sync([n,&v,db]()->void{
            SqliteStorage::get(db.operator->(),n.c_str(),v);
        });
        return v;
    }
    
    void SqliteStorage::set(kk::CString key,kk::CString value) {
        
        if(key == nullptr) {
            return;
        }
        
        kk::String n(_prefix);
        n.append(key);
        kk::String v;
        if(value) {
            v.append(value);
        }
        kk::Strong<Sqlite> db = _db;
        _queue->async([n,value,db,v]()->void{
            SqliteStorage::set(db.operator->(),n.c_str(), v.empty() ? nullptr : v.c_str() );
        });
    }
    
    void SqliteStorage::clear() {
        kk::String p(_prefix);
        kk::Strong<Sqlite> db = _db;
        _queue->async([p,db]()->void{
            SqliteStorage::clear(db.operator->(),p.c_str());
        });
    }
    
    void SqliteStorage::load(kk::CString key,JSObject * callback) {
        
        if(callback == nullptr) {
            return;
        }
        
        if(key == nullptr) {
            if(callback) {
                kk::Any v;
                callback->invoke<void,kk::Any &>(nullptr, v);
            }
            return;
        }
        
        kk::Strong<DispatchQueue> queue = getCurrentDispatchQueue();
        kk::Strong<JSObject> cb = callback;
        kk::String n(_prefix);
        n.append(key);
        kk::Any v;
        kk::Strong<Sqlite> db = _db;
        _queue->async([n,db,cb,queue]()->void{
            kk::String v;
            SqliteStorage::get(db.operator->(),n.c_str(),v);
            queue->async([v,cb]()->void{
                JSObject * callback = cb.operator->();
                if(callback) {
                    callback->invoke<void,kk::CString>(nullptr, v.c_str());
                }
            });
        });
    }
    
    kk::Strong<kk::Array<kk::String>> SqliteStorage::keys() {
        kk::String n(_prefix);
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
        kk::String n(_prefix);
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
    
    void SqliteStorage::get(Sqlite * db,kk::CString key,kk::String & v) {
        
        if(key == nullptr) {
            return;
        }
        
        try {
            
            kk::Strong<kk::Array<kk::Any>> data = new kk::Array<kk::Any>({key});
            
            auto rs = db->query("SELECT value FROM Storage WHERE key=?", (kk::Array<kk::Any> *) data);
            
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
    
    void SqliteStorage::set(Sqlite * db,kk::CString key,kk::CString v) {
        
        if(key == nullptr) {
            return;
        }
        
        try {
            
            kk::Boolean hasKey = false;
            
            kk::Strong<kk::Array<kk::Any>> data = new kk::Array<kk::Any>({key});
            
            auto rs = db->query("SELECT COUNT(*) FROM Storage WHERE key=?", (kk::Array<kk::Any> *) data);
            
            if(rs->next()) {
                kk::Any v = rs->getValue(0);
                hasKey = ((kk::Int32) v) > 0;
            }
        
            rs->close();
            
            if(hasKey) {
                if(v == nullptr) {
                    db->exec("DELETE FROM Storage WHERE key=?", (kk::Array<kk::Any> *) data);
                } else {
                    data = new kk::Array<kk::Any>({v,key});
                    db->exec("UPDATE Storage SET value=? WHERE key=?", (kk::Array<kk::Any> *) data);
                }
            } else if(v != nullptr) {
                data = new kk::Array<kk::Any>({key,v});
                db->exec("INSERT INTO Storage(key,value) VALUES(?,?)", (kk::Array<kk::Any> *) data);
            }
            
        } catch(kk::CString errmsg) {
            kk::Log("[SqliteStorage::set] %s",errmsg);
        }
    }
    
    void SqliteStorage::clear(Sqlite * db,kk::CString prefix) {
        
        try {
            kk::String p = prefix;
            p.append("%");
            
            db->exec("DELETE FROM Storage WHERE key LIKE ?", new kk::Array<kk::Any>({p.c_str()}));
            
        } catch(kk::CString errmsg) {
            kk::Log("[SqliteStorage::set] %s",errmsg);
        }
        
    }
    
    void SqliteStorage::keys(Sqlite * db,kk::CString prefix,kk::Array<kk::String> * keys) {
        
        try {
            
            kk::String p = prefix;
            p.append("%");
            
            kk::Strong<kk::Array<kk::Any>> data = new kk::Array<kk::Any>({p.c_str()});
            
            auto rs = db->query("SELECT key FROM Storage WHERE key LIKE ?", (kk::Array<kk::Any> *) data);
            
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
    
    void SqliteStorage::install(Sqlite * db) {
        try {
            db->exec("CREATE TABLE IF NOT EXISTS Storage(key VARCHAR(4096) PRIMARY KEY , value TEXT)", (kk::Array<kk::Any> *) nullptr);
        }
        catch(kk::CString errmsg) {
            kk::Log("[SqliteStorage::install] %s",errmsg);
        }
    }
    
    
    
    
}
