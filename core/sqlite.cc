//
//  sqlite.cc
//  Ker
//
//  Created by hailong11 on 2018/12/11.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#include <core/sqlite.h>

namespace kk {
    
    SqliteResultSet::SqliteResultSet(sqlite3_stmt * stmt):_stmt(stmt) {
        
    }
    
    SqliteResultSet::~SqliteResultSet() {
        if(_stmt) {
            sqlite3_finalize(_stmt);
        }
    }
    
    kk::Uint SqliteResultSet::getColumnCount() {
        if(_stmt) {
            return (kk::Uint) sqlite3_column_count(_stmt);
        }
        return 0;
    }
    
    kk::CString SqliteResultSet::getColumnName(kk::Int idx) {
        if(_stmt) {
            return sqlite3_column_name(_stmt, idx);
        }
        return nullptr;
    }
    
    kk::Boolean SqliteResultSet::next() {
        if(_stmt) {
            return sqlite3_step(_stmt) == SQLITE_ROW;
        }
        return false;
    }
    
    kk::Any SqliteResultSet::getValue(kk::Int idx) {
        kk::Any v;
        if(_stmt) {
            switch (sqlite3_column_type(_stmt, idx)) {
                case SQLITE_INTEGER:
                    v = (kk::Int) sqlite3_column_int(_stmt, idx);
                    break;
                case SQLITE_FLOAT:
                    v = (kk::Double) sqlite3_column_double(_stmt, idx);
                    break;
                case SQLITE_TEXT:
                    v = (kk::CString) sqlite3_column_text(_stmt, idx);
                    break;
                case SQLITE_BLOB:
                    v = new kk::ArrayBuffer((void *) sqlite3_column_blob(_stmt, idx)
                                            ,sqlite3_column_bytes(_stmt, idx));
                    break;
                default:
                    break;
            }
        }
        return v;
    }
    
    void SqliteResultSet::close() {
        if(_stmt) {
            sqlite3_finalize(_stmt);
            _stmt = nullptr;
        }
    }
    
    Sqlite::Sqlite():_db(nullptr) {
    
    }
    
    Sqlite::~Sqlite() {
        if(_db) {
            sqlite3_close(_db);
            _db = nullptr;
        }
    }
    
    void Sqlite::open(kk::CString path) throw(kk::CString) {
        if(_db == nullptr) {
            
            if(SQLITE_OK != sqlite3_open(path, &_db)) {
                throw "打开数据库失败";
            }
            
        } else {
            throw "数据库已经打开";
        }
    }
  
    static void SqliteBind(sqlite3_stmt * st,kk::Array<kk::Any> * data) {
        
        if(data == nullptr) {
            return;
        }
        
        int n = sqlite3_bind_parameter_count(st);
        auto p = data->begin();
        
        for(int i=0;i<n;i++) {
            if(data->end() == p) {
                sqlite3_bind_null(st, i + 1);
            } else {
                kk::Any & v = * p;
                switch (v.type) {
                    case kk::TypeInt8:
                        sqlite3_bind_int(st, i + 1, v.int8Value);
                        break;
                    case kk::TypeInt16:
                        sqlite3_bind_int(st, i + 1, v.int16Value);
                        break;
                    case kk::TypeInt32:
                        sqlite3_bind_int(st, i + 1, v.int32Value);
                        break;
                    case kk::TypeInt64:
                        sqlite3_bind_int64(st, i + 1, v.int64Value);
                        break;
                    case kk::TypeUint8:
                        sqlite3_bind_int(st, i + 1, v.uint8Value);
                        break;
                    case kk::TypeUint16:
                        sqlite3_bind_int(st, i + 1, v.uint16Value);
                        break;
                    case kk::TypeUint32:
                        sqlite3_bind_int(st, i + 1, v.uint32Value);
                        break;
                    case kk::TypeUint64:
                        sqlite3_bind_int64(st, i + 1, v.uint64Value);
                        break;
                    case kk::TypeFloat32:
                        sqlite3_bind_double(st, i + 1, v.float32Value);
                        break;
                    case kk::TypeFloat64:
                        sqlite3_bind_double(st, i + 1, v.float64Value);
                        break;
                    case kk::TypeBoolean:
                        sqlite3_bind_int(st, i + 1, v.booleanValue ? 1 : 0);
                        break;
                    case kk::TypeString:
                        sqlite3_bind_text(st, i + 1, v.stringValue, (int) v.length, SQLITE_STATIC);
                        break;
                    case kk::TypeObject:
                    {
                        ArrayBuffer * b = v;
                        if(b) {
                            sqlite3_bind_blob(st, i + 1, b->data(), b->byteLength(), SQLITE_STATIC);
                        }
                    }
                        break;
                    default:
                        break;
                }
                p ++;
            }
        }
    }
    
    static void SqliteBindValue(sqlite3_stmt * st,int i, duk_context * ctx,duk_idx_t idx) {
        switch (duk_get_type(ctx, idx)) {
            case DUK_TYPE_NUMBER:
                sqlite3_bind_double(st, i, duk_to_number(ctx, idx));
                break;
            case DUK_TYPE_BOOLEAN:
                sqlite3_bind_int(st, i, duk_to_boolean(ctx, idx) ? 1 : 0);
                break;
            case DUK_TYPE_STRING:
                sqlite3_bind_text(st, i, duk_to_string(ctx, idx), -1,SQLITE_STATIC);
                break;
            case DUK_TYPE_BUFFER:
                if(duk_is_buffer_data(ctx, -1)) {
                    size_t n =0;
                    void * data = duk_get_buffer_data(ctx, idx, &n);
                    sqlite3_bind_blob(st, i, data, (int) n, SQLITE_STATIC);
                } else {
                    size_t n =0;
                    void * data = duk_get_buffer(ctx, idx, &n);
                    sqlite3_bind_blob(st, i, data, (int) n, SQLITE_STATIC);
                }
                break;
            default:
                sqlite3_bind_null(st, i);
                break;
        }
    }
    
    static void SqliteBind(sqlite3_stmt * st,kk::JSObject * data) {
        
        if(data == nullptr) {
            return;
        }
        
        duk_context * ctx = data->jsContext();
        void * heapptr = data->heapptr();
        
        if(ctx && heapptr) {
            
            duk_push_heapptr(ctx, heapptr);
            
            if(duk_is_array(ctx, -1)){
                
                int n = sqlite3_bind_parameter_count(st);
  
                for(int i=0;i<n;i++) {
                    duk_get_prop_index(ctx, -1, i);
                    SqliteBindValue(st,i + 1,ctx,-1);
                    duk_pop(ctx);
                }
                
            } else if(duk_is_object(ctx, -1)) {
            
                int n = sqlite3_bind_parameter_count(st);
                
                for(int i=0;i<n;i++) {
                    CString name = sqlite3_bind_parameter_name(st, i + 1);
                    if(kk::CStringHasPrefix(name, "@")) {
                        duk_get_prop_string(ctx, -1, name + 1);
                        SqliteBindValue(st,i + 1,ctx,-1);
                        duk_pop(ctx);
                    } else {
                        sqlite3_bind_null(st, i + 1);
                    }
                }
            }
            
            duk_pop(ctx);
            
        }
        
    }
    
    kk::Strong<SqliteResultSet> Sqlite::query(kk::CString sql,kk::Array<kk::Any> * data) throw(kk::CString) {
        
        if(_db == nullptr) {
            throw "未打开数据库";
        }
        
        sqlite3_stmt * st = nullptr;
        
        if(SQLITE_OK != sqlite3_prepare(_db, sql, -1, &st, nullptr)) {
            throw sqlite3_errmsg(_db);
        }
        
        SqliteBind(st,data);
        
        return new SqliteResultSet(st);
    }
    
    kk::Strong<SqliteResultSet> Sqlite::query(kk::CString sql,kk::JSObject * data) throw(kk::CString) {
        
        if(_db == nullptr) {
            throw "未打开数据库";
        }
        
        sqlite3_stmt * st = nullptr;
        
        if(SQLITE_OK != sqlite3_prepare(_db, sql, -1, &st, nullptr)) {
            throw sqlite3_errmsg(_db);
        }
        
        SqliteBind(st,data);
        
        return new SqliteResultSet(st);
        
    }
    
    void Sqlite::exec(kk::CString sql,kk::Array<kk::Any> * data) throw(kk::CString) {
        
        if(_db == nullptr) {
            throw "未打开数据库";
        }
        
        sqlite3_stmt * st = nullptr;
        
        if(SQLITE_OK != sqlite3_prepare(_db, sql, -1, &st, nullptr)) {
            throw sqlite3_errmsg(_db);
        }
        
        SqliteBind(st,data);
        
        if(sqlite3_step(st) == SQLITE_ERROR) {
            sqlite3_finalize(st);
            throw sqlite3_errmsg(_db);
        }
        
        sqlite3_finalize(st);
    }
    
    void Sqlite::exec(kk::CString sql,kk::JSObject * data) throw(kk::CString) {
        
        if(_db == nullptr) {
            throw "未打开数据库";
        }
        
        sqlite3_stmt * st = nullptr;
        
        if(SQLITE_OK != sqlite3_prepare(_db, sql, -1, &st, nullptr)) {
            throw sqlite3_errmsg(_db);
        }
        
        SqliteBind(st,data);
        
        if(sqlite3_step(st) == SQLITE_ERROR) {
            sqlite3_finalize(st);
            throw sqlite3_errmsg(_db);
        }
        
        sqlite3_finalize(st);
        
    }
    
    kk::Int64 Sqlite::lastInsertRowId() {
        if(_db) {
            return sqlite3_last_insert_rowid(_db);
        }
        return 0;
    }
    

    void Sqlite::close() {
        if(_db) {
            sqlite3_close(_db);
            _db = nullptr;
        }
    }
    
    void Sqlite::Openlib() {
        
        kk::Openlib<>::add([](duk_context * ctx)->void{
            
            kk::PushInterface<Sqlite>(ctx, [](duk_context * ctx)->void{
                
                kk::PutMethod<Sqlite,void,kk::CString,JSObject *>(ctx, -1, "exec", &Sqlite::exec);
                
                kk::PutStrongMethod<Sqlite,SqliteResultSet,kk::CString,JSObject *>(ctx, -1, "query", &Sqlite::query);
                
                kk::PutMethod<Sqlite,kk::Int64>(ctx, -1, "lastInsertRowId", &Sqlite::lastInsertRowId);
                
                kk::PutMethod<Sqlite,void>(ctx, -1, "close", &Sqlite::close);
                
            });
            
            kk::PushInterface<SqliteResultSet>(ctx, [](duk_context * ctx)->void{
                
                kk::PutMethod<SqliteResultSet,kk::Uint>(ctx, -1, "getColumnCount", &SqliteResultSet::getColumnCount);
                
                kk::PutMethod<SqliteResultSet,kk::CString,kk::Int>(ctx, -1, "getColumnName", &SqliteResultSet::getColumnName);
                
                kk::PutMethod<SqliteResultSet,kk::Any,kk::Int>(ctx, -1, "getValue", &SqliteResultSet::getValue);
                
                kk::PutMethod<SqliteResultSet,kk::Boolean>(ctx, -1, "next", &SqliteResultSet::next);
                
                kk::PutMethod<SqliteResultSet,void>(ctx, -1, "close", &SqliteResultSet::close);
                
            });
            
        });
        
    }
    
    
}

