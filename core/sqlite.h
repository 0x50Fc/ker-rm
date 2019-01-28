//
//  sqlite.h
//  Ker
//
//  Created by zhanghailong on 2018/12/11.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#ifndef kk_sqlite_h
#define kk_sqlite_h

#include <core/kk.h>
#include <core/jit.h>
#include <sqlite3.h>

namespace kk {
    
    class Database : public Object {
    public:
        virtual void exec(kk::CString sql,kk::Array<kk::Any> * data,std::function<void(kk::Int64,kk::CString)> && func) = 0;
        virtual void query(kk::CString sql,kk::Array<kk::Any> * data,std::function<void(kk::Array<kk::Any> *,kk::CString)> && func) = 0;
        virtual void close() = 0;
        virtual void exec(kk::CString sql,JSObject * data,JSObject * func);
        virtual void query(kk::CString sql,JSObject * data,JSObject * func);
        
        Ker_CLASS(Database, Object, "Database")
        
        static void Openlib();
    };
    
    class SqliteResultSet : public Object {
    public:
        SqliteResultSet(sqlite3_stmt * stmt);
        virtual ~SqliteResultSet();
        virtual kk::Uint getColumnCount();
        virtual kk::CString getColumnName(kk::Int idx);
        virtual kk::Boolean next();
        virtual kk::Any getValue(kk::Int idx);
        virtual void close();
        
        Ker_CLASS(SqliteResultSet,Object,"SqliteResultSet")
        
    protected:
        sqlite3_stmt * _stmt;
    };
    
    class Sqlite : public Object {
    public:
        
        Sqlite();
        
        virtual ~Sqlite();
        
        virtual void open(kk::CString path) throw(kk::CString);
        
        virtual kk::Strong<SqliteResultSet> query(kk::CString sql,kk::Array<kk::Any> * data) throw(kk::CString);
        
        virtual kk::Strong<SqliteResultSet> query(kk::CString sql,kk::JSObject * data) throw(kk::CString);
        
        virtual void exec(kk::CString sql,kk::Array<kk::Any> * data) throw(kk::CString);
        
        virtual void exec(kk::CString sql,kk::JSObject * data) throw(kk::CString);
        
        virtual kk::Int64 lastInsertRowId();
        
        virtual void close();
        
        Ker_CLASS(Sqlite,Object,"Sqlite")
        
        static void Openlib();
        
    protected:
        sqlite3 * _db;
    };
    
    class SqliteDatabase : public Database {
    public:
        SqliteDatabase(Sqlite * sqlite);
        virtual void exec(kk::CString sql,kk::Array<kk::Any> * data,std::function<void(kk::Int64,kk::CString)> && func);
        virtual void query(kk::CString sql,kk::Array<kk::Any> * data,std::function<void(kk::Array<kk::Any> *,kk::CString)> && func);
        virtual void close();
        
        Ker_CLASS(SqliteDatabase, Database, "SqliteDatabase")
        
        static void Openlib();
        
    protected:
        kk::Strong<Sqlite> _sqlite;
    };
    
}

#endif /* sqlite_h */
