var ker;
(function (ker) {
    var DBIndexType;
    (function (DBIndexType) {
        DBIndexType[DBIndexType["NONE"] = 0] = "NONE";
        DBIndexType[DBIndexType["ASC"] = 1] = "ASC";
        DBIndexType[DBIndexType["DESC"] = 2] = "DESC";
    })(DBIndexType = ker.DBIndexType || (ker.DBIndexType = {}));
    var DBFieldType;
    (function (DBFieldType) {
        DBFieldType[DBFieldType["VARCHAR"] = 0] = "VARCHAR";
        DBFieldType[DBFieldType["INT"] = 1] = "INT";
        DBFieldType[DBFieldType["TINYINT"] = 2] = "TINYINT";
        DBFieldType[DBFieldType["BIGINT"] = 3] = "BIGINT";
        DBFieldType[DBFieldType["DOUBLE"] = 4] = "DOUBLE";
        DBFieldType[DBFieldType["TEXT"] = 5] = "TEXT";
        DBFieldType[DBFieldType["BLOB"] = 6] = "BLOB";
    })(DBFieldType = ker.DBFieldType || (ker.DBFieldType = {}));
    var DBCommandType;
    (function (DBCommandType) {
        DBCommandType[DBCommandType["ADD"] = 0] = "ADD";
        DBCommandType[DBCommandType["SET"] = 1] = "SET";
        DBCommandType[DBCommandType["REMOVE"] = 2] = "REMOVE";
    })(DBCommandType = ker.DBCommandType || (ker.DBCommandType = {}));
    function DBSQLDefaultValue(fd) {
        switch (fd.type) {
            case DBFieldType.VARCHAR:
                if (fd.default !== undefined) {
                    return JSON.stringify(fd.default + '');
                }
                break;
            case DBFieldType.INT:
            case DBFieldType.TINYINT:
            case DBFieldType.BIGINT:
            case DBFieldType.DOUBLE:
                if (fd.default !== undefined) {
                    return fd.default + '';
                }
                break;
        }
        return 'NULL';
    }
    function DBSQLType(fd) {
        var type = 'VARCHAR';
        switch (fd.type) {
            case DBFieldType.VARCHAR:
                type = 'VARCHAR';
                break;
            case DBFieldType.INT:
                type = 'INT';
                break;
            case DBFieldType.INT:
                type = 'TINYINT';
                break;
            case DBFieldType.BIGINT:
                type = 'BIGINT';
                break;
            case DBFieldType.DOUBLE:
                type = 'DOUBLE';
                break;
            case DBFieldType.TEXT:
                type = 'TEXT';
                break;
            case DBFieldType.BLOB:
                type = 'BLOB';
                break;
        }
        if (fd.length > 0) {
            type += '(' + fd.length + ')';
        }
        return type;
    }
    var DBContext = /** @class */ (function () {
        function DBContext(db) {
            this._emitter = new EventEmitter();
            this._db = db;
            this._db.exec("CREATE TABLE IF NOT EXISTS __entrys(name VARCHAR(255) PRIMARY KEY , value TEXT)", [], function (id, errmsg) {
                if (errmsg !== undefined) {
                    console.error("[DBContext] [constructor]", errmsg);
                }
            });
        }
        DBContext.prototype.on = function (name, func) {
            this._emitter.on(name, func);
        };
        DBContext.prototype.off = function (name, func) {
            this._emitter.off(name, func);
        };
        DBContext.prototype.has = function (name) {
            return this._emitter.has(name);
        };
        DBContext.prototype.emit = function (name, event) {
            this._emitter.emit(name, event);
        };
        Object.defineProperty(DBContext.prototype, "db", {
            get: function () {
                return this._db;
            },
            enumerable: true,
            configurable: true
        });
        DBContext.prototype.addEntry = function (entry) {
            var _this = this;
            this._db.query("SELECT * FROM __entrys WHERE name=?", [entry.name], function (items, errmsg) {
                if (errmsg !== undefined) {
                    console.error("[DBContext] [addEntry]", errmsg);
                }
                else if (items && items.length > 0) {
                    var e = JSON.parse(items[0]['value']);
                    var fds = {};
                    var sql = [];
                    for (var _i = 0, _a = e.fields; _i < _a.length; _i++) {
                        var fd = _a[_i];
                        fds[fd.name] = fd;
                    }
                    for (var _b = 0, _c = entry.fields; _b < _c.length; _b++) {
                        var fd = _c[_b];
                        var f = fds[fd.name];
                        if (f === undefined) {
                            sql.push('ALTER TABLE [');
                            sql.push("entry.name");
                            sql.push("] ADD [");
                            sql.push(fd.name);
                            sql.push("] ");
                            sql.push(DBSQLType(fd));
                            sql.push(" DEFAULT ");
                            sql.push(DBSQLDefaultValue(fd));
                            sql.push("; ");
                        }
                        else if (f.type != fd.type || f.length != fd.length) {
                            sql.push('ALTER TABLE [');
                            sql.push("entry.name");
                            sql.push("] CHANGE [");
                            sql.push(fd.name);
                            sql.push("] [");
                            sql.push(fd.name);
                            sql.push("] ");
                            sql.push(DBSQLType(fd));
                            sql.push(" DEFAULT ");
                            sql.push(DBSQLDefaultValue(fd));
                            sql.push("; ");
                        }
                        else if (fd.index != DBIndexType.NONE && f.index == DBIndexType.NONE) {
                            sql.push('CREATE INDEX [');
                            sql.push(entry.name);
                            sql.push('_');
                            sql.push(fd.name);
                            sql.push('_idx] ON [');
                            sql.push(entry.name);
                            sql.push(']([');
                            sql.push(fd.name);
                            sql.push('] ');
                            if (fd.index == DBIndexType.DESC) {
                                sql.push('DESC');
                            }
                            else {
                                sql.push('ASC');
                            }
                            sql.push(');');
                        }
                    }
                    if (sql.length > 0) {
                        console.info("[SQL]", sql.join(''));
                        _this._db.exec(sql.join(''), [], function (id, errmsg) {
                            if (errmsg !== undefined) {
                                console.error("[DBContext] [addEntry]", errmsg);
                            }
                        });
                        console.info("[SQL]", "UPDATE __entrys SET value=? WHERE key=?;");
                        _this._db.exec("UPDATE __entrys SET value=? WHERE key=?;", [JSON.stringify(entry), entry.name], function (id, errmsg) {
                            if (errmsg !== undefined) {
                                console.error("[DBContext] [addEntry]", errmsg);
                            }
                        });
                    }
                }
                else {
                    var sql = [];
                    sql.push("CREATE TABLE IF NOT EXISTS [");
                    sql.push(entry.name);
                    sql.push("](id INTEGER PRIMARY KEY AUTOINCREMENT");
                    for (var _d = 0, _e = entry.fields; _d < _e.length; _d++) {
                        var fd = _e[_d];
                        sql.push(",[");
                        sql.push(fd.name);
                        sql.push('] ');
                        sql.push(DBSQLType(fd));
                        sql.push(" DEFAULT ");
                        sql.push(DBSQLDefaultValue(fd));
                    }
                    sql.push(");");
                    console.info("[SQL]", sql.join(''));
                    _this._db.exec(sql.join(''), [], function (id, errmsg) {
                        if (errmsg !== undefined) {
                            console.error("[DBContext] [addEntry]", errmsg);
                        }
                    });
                    console.info("[SQL]", "INSERT INTO __entrys(name,value) VALUES(?,?);");
                    _this._db.exec("INSERT INTO __entrys(name,value) VALUES(?,?);", [entry.name, JSON.stringify(entry)], function (id, errmsg) {
                        if (errmsg !== undefined) {
                            console.error("[DBContext] [addEntry]", errmsg);
                        }
                    });
                }
            });
        };
        DBContext.prototype.query = function (sql, data, done) {
            this._db.query(sql, data, done);
        };
        DBContext.prototype.exec = function (sql, data, done) {
            this._db.exec(sql, data, done);
        };
        DBContext.prototype.queryEntry = function (entry, sql, data, done) {
            this._db.query(['SELECT * FROM [', entry.name, '] ', sql].join(''), data, done);
        };
        DBContext.prototype.add = function (object, entry, done) {
            var _this = this;
            var sql = ['INSERT INTO [', entry.name, ']('];
            var names = [];
            var valus = [];
            var vs = [];
            for (var _i = 0, _a = entry.fields; _i < _a.length; _i++) {
                var fd = _a[_i];
                names.push('[' + fd.name + ']');
                valus.push('?');
                var v = object[fd.name];
                if (v === undefined) {
                    vs.push(fd.default);
                }
                else {
                    vs.push(v);
                }
            }
            sql.push(names.join(','));
            sql.push(') VALUES(');
            sql.push(valus.join(','));
            sql.push(')');
            console.info("[SQL]", sql.join(''), vs);
            this._db.exec(sql.join(''), vs, function (id, errmsg) {
                if (errmsg === undefined) {
                    object.id = id;
                    var e = new Event();
                    e.data = {
                        type: DBCommandType.ADD,
                        object: object,
                        entry: entry
                    };
                    _this.emit("change", e);
                }
                if (done !== undefined) {
                    done(errmsg);
                }
            });
        };
        DBContext.prototype.remove = function (objects, entry, done) {
            var _this = this;
            var sql = ['DELETE FROM [', entry.name, '] FROM [id] IN ('];
            var vs = [];
            var as = [];
            for (var _i = 0, objects_1 = objects; _i < objects_1.length; _i++) {
                var v = objects_1[_i];
                as.push("?");
                vs.push(v.id);
            }
            sql.push(as.join(','));
            sql.push(")");
            console.info("[SQL]", sql.join(''), vs);
            this._db.exec(sql.join(''), vs, function (id, errmsg) {
                if (errmsg === undefined) {
                    var e = new Event();
                    e.data = {
                        type: DBCommandType.REMOVE,
                        objects: objects,
                        entry: entry
                    };
                    _this.emit("change", e);
                }
                if (done !== undefined) {
                    done(errmsg);
                }
            });
        };
        DBContext.prototype.set = function (object, entry, keys, done) {
            var _this = this;
            var sql = ['UPDATE [', entry.name, '] SET '];
            var items = [];
            var vs = [];
            if (keys === undefined) {
                keys = [];
                for (var _i = 0, _a = entry.fields; _i < _a.length; _i++) {
                    var fd = _a[_i];
                    keys.push(fd.name);
                }
            }
            var defaultValues = {};
            for (var _b = 0, _c = entry.fields; _b < _c.length; _b++) {
                var fd = _c[_b];
                defaultValues[fd.name] = fd.default;
            }
            for (var _d = 0, keys_1 = keys; _d < keys_1.length; _d++) {
                var key = keys_1[_d];
                var v = object[key];
                if (v === undefined) {
                    v = defaultValues[key];
                }
                vs.push(v);
                items.push('[' + key + ']=?');
            }
            sql.push(items.join(","));
            sql.push(" WHERE [id]=?");
            vs.push(object.id);
            console.info("[SQL]", sql.join(''), vs);
            this._db.exec(sql.join(''), vs, function (id, errmsg) {
                if (errmsg === undefined) {
                    var e = new Event();
                    e.data = {
                        type: DBCommandType.SET,
                        object: object,
                        entry: entry,
                        keys: keys
                    };
                    _this.emit("change", e);
                }
                if (done !== undefined) {
                    done(errmsg);
                }
            });
        };
        return DBContext;
    }());
    ker.DBContext = DBContext;
})(ker || (ker = {}));
