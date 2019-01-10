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
}

#endif /* sqlite_h */
