namespace ker {

    export enum DBIndexType {
        NONE, ASC, DESC
    }

    export enum DBFieldType {
        VARCHAR, INT, TINYINT, BIGINT, DOUBLE, TEXT, BLOB
    }

    export interface DBField {
        readonly name: string
        readonly type: DBFieldType
        readonly index: DBIndexType
        readonly length: number
        readonly default: DatabaseValue
    }

    export interface DBEntry {
        readonly name: string
        readonly fields: DBField[]
    }

    export interface DBObject {
        id: number
        [key: string]: DatabaseValue
    }

    export enum DBCommandType {
        ADD, SET, REMOVE
    }

    export interface DBCommand {
        readonly type: DBCommandType
    }

    export interface DBAddCommand extends DBCommand {
        readonly object: DBObject
        readonly entry: DBEntry
    }

    export interface DBSetCommand extends DBCommand {
        readonly object: DBObject
        readonly entry: DBEntry
        readonly keys: string[]
    }

    export interface DBRemoveCommand extends DBCommand {
        readonly objects: DBObject[]
        readonly entry: DBEntry
    }

    export interface DBFieldSet {
        [key: string]: DBField
    }

    function DBSQLDefaultValue(fd: DBField): string {
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

    function DBSQLType(fd: DBField): string {
        let type: string = 'VARCHAR'
        switch (fd.type) {
            case DBFieldType.VARCHAR:
                type = 'VARCHAR'
                break;
            case DBFieldType.INT:
                type = 'INT'
                break;
            case DBFieldType.INT:
                type = 'TINYINT'
                break;
            case DBFieldType.BIGINT:
                type = 'BIGINT'
                break;
            case DBFieldType.DOUBLE:
                type = 'DOUBLE'
                break;
            case DBFieldType.TEXT:
                type = 'TEXT'
                break;
            case DBFieldType.BLOB:
                type = 'BLOB'
                break;
        }
        if (fd.length > 0) {
            type += '(' + fd.length + ')';
        }
        return type;
    }

    export class DBContext {

        private _emitter: EventEmitter;
        private _db: Database;

        constructor(db: Database) {
            this._emitter = new EventEmitter();
            this._db = db;
            this._db.exec("CREATE TABLE IF NOT EXISTS __entrys(name VARCHAR(255) PRIMARY KEY , value TEXT)", [], (id: number, errmsg: string | undefined): void => {
                if (errmsg !== undefined) {
                    console.error("[DBContext] [constructor]", errmsg);
                }
            });
        }

        on(name: string, func: EventFunction): void {
            this._emitter.on(name, func);
        }
        off(name?: string, func?: EventFunction): void {
            this._emitter.off(name, func);
        }
        has(name: string): boolean {
            return this._emitter.has(name);
        }
        emit(name: string, event: Event): void {
            this._emitter.emit(name, event);
        }

        get db(): Database {
            return this._db;
        }

        addEntry(entry: DBEntry): void {
            this._db.query("SELECT * FROM __entrys WHERE name=?", [entry.name], (items: DatabaseRow[], errmsg: string | undefined): void => {
                if (errmsg !== undefined) {
                    console.error("[DBContext] [addEntry]", errmsg);
                } else if (items && items.length > 0) {
                    let e: DBEntry = JSON.parse(items[0]['value'] as string);
                    let fds: DBFieldSet = {};
                    let sql: string[] = [];

                    for (let fd of e.fields) {
                        fds[fd.name] = fd;
                    }

                    for (let fd of entry.fields) {
                        let f = fds[fd.name];
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
                        } else if (f.type != fd.type || f.length != fd.length) {
                            sql.push('ALTER TABLE [');
                            sql.push("entry.name");
                            sql.push("] CHANGE [");
                            sql.push(fd.name);
                            sql.push("] [");
                            sql.push(fd.name);
                            sql.push("] ")
                            sql.push(DBSQLType(fd));
                            sql.push(" DEFAULT ");
                            sql.push(DBSQLDefaultValue(fd));
                            sql.push("; ");
                        } else if (fd.index != DBIndexType.NONE && f.index == DBIndexType.NONE) {
                            sql.push('CREATE INDEX [');
                            sql.push(entry.name)
                            sql.push('_')
                            sql.push(fd.name)
                            sql.push('_idx] ON [')
                            sql.push(entry.name)
                            sql.push(']([');
                            sql.push(fd.name)
                            sql.push('] ');
                            if (fd.index == DBIndexType.DESC) {
                                sql.push('DESC');
                            } else {
                                sql.push('ASC');
                            }
                            sql.push(');');
                        }
                    }

                    if (sql.length > 0) {

                        console.info("[SQL]", sql.join(''));
                        this._db.exec(sql.join(''),
                            [],
                            (id: number, errmsg: string | undefined): void => {
                                if (errmsg !== undefined) {
                                    console.error("[DBContext] [addEntry]", errmsg);
                                }
                            });

                        console.info("[SQL]", "UPDATE __entrys SET value=? WHERE key=?;");
                        this._db.exec("UPDATE __entrys SET value=? WHERE key=?;",
                            [JSON.stringify(entry), entry.name],
                            (id: number, errmsg: string | undefined): void => {
                                if (errmsg !== undefined) {
                                    console.error("[DBContext] [addEntry]", errmsg);
                                }
                            });

                    }

                } else {

                    let sql: string[] = [];

                    sql.push("CREATE TABLE IF NOT EXISTS [");
                    sql.push(entry.name);
                    sql.push("](id INTEGER PRIMARY KEY AUTOINCREMENT");

                    for (let fd of entry.fields) {
                        sql.push(",[");
                        sql.push(fd.name);
                        sql.push('] ');
                        sql.push(DBSQLType(fd));
                        sql.push(" DEFAULT ");
                        sql.push(DBSQLDefaultValue(fd));
                    }

                    sql.push(");")

                    console.info("[SQL]", sql.join(''));

                    this._db.exec(sql.join(''),
                        [],
                        (id: number, errmsg: string | undefined): void => {
                            if (errmsg !== undefined) {
                                console.error("[DBContext] [addEntry]", errmsg);
                            }
                        });

                    console.info("[SQL]", "INSERT INTO __entrys(name,value) VALUES(?,?);");

                    this._db.exec("INSERT INTO __entrys(name,value) VALUES(?,?);",
                        [entry.name, JSON.stringify(entry)],
                        (id: number, errmsg: string | undefined): void => {
                            if (errmsg !== undefined) {
                                console.error("[DBContext] [addEntry]", errmsg);
                            }
                        });
                }
            });
        }

        query(sql: string, data: DatabaseValue[], done: (objects: DatabaseRow[], errmsg: string | undefined) => void): void {
            this._db.query(sql, data, done);
        }

        exec(sql: string, data: DatabaseValue[], done: (id: number, errmsg: string | undefined) => void): void {
            this._db.exec(sql, data, done);
        }

        queryEntry(entry: DBEntry, sql: string, data: DatabaseValue[], done: (objects: DBObject[], errmsg: string | undefined) => void): void {
            this._db.query(['SELECT * FROM [', entry.name, '] ', sql].join(''), data, done);
        }

        add(object: DBObject, entry: DBEntry, done?: (errmsg: string | undefined) => void): void {

            let sql = ['INSERT INTO [', entry.name, ']('];
            let names: string[] = [];
            let valus: string[] = [];
            let vs: DatabaseValue[] = [];

            for (let fd of entry.fields) {
                names.push('[' + fd.name + ']');
                valus.push('?');
                let v = object[fd.name];
                if (v === undefined) {
                    vs.push(fd.default);
                } else {
                    vs.push(v);
                }
            }

            sql.push(names.join(','));
            sql.push(') VALUES(');
            sql.push(valus.join(','));
            sql.push(')')

            console.info("[SQL]", sql.join(''), vs);

            this._db.exec(sql.join(''), vs, (id: number, errmsg: string | undefined): void => {

                if (errmsg === undefined) {
                    object.id = id;
                    let e = new Event();
                    e.data = {
                        type: DBCommandType.ADD,
                        object: object,
                        entry: entry
                    };
                    this.emit("change", e);
                }
                if (done !== undefined) {
                    done(errmsg);
                }
            });

        }

        remove(objects: DBObject[], entry: DBEntry, done?: (errmsg: string | undefined) => void): void {

            let sql = ['DELETE FROM [', entry.name, '] FROM [id] IN ('];
            let vs: DatabaseValue[] = [];
            let as: string[] = [];

            for (let v of objects) {
                as.push("?");
                vs.push(v.id);
            }

            sql.push(as.join(','))
            sql.push(")")

            console.info("[SQL]", sql.join(''), vs);

            this._db.exec(sql.join(''), vs, (id: number, errmsg: string | undefined): void => {

                if (errmsg === undefined) {
                    let e = new Event();
                    e.data = {
                        type: DBCommandType.REMOVE,
                        objects: objects,
                        entry: entry
                    };
                    this.emit("change", e);
                }
                if (done !== undefined) {
                    done(errmsg);
                }
            });

        }

        set(object: DBObject, entry: DBEntry, keys?: string[] | undefined, done?: (errmsg: string | undefined) => void): void {

            let sql = ['UPDATE [', entry.name, '] SET '];
            let items: string[] = [];
            let vs: DatabaseValue[] = [];

            if (keys === undefined) {
                keys = [];
                for (let fd of entry.fields) {
                    keys.push(fd.name);
                }
            }

            let defaultValues: DatabaseRow = {};

            for (let fd of entry.fields) {
                defaultValues[fd.name] = fd.default;
            }

            for (let key of keys) {
                let v = object[key];
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

            this._db.exec(sql.join(''), vs, (id: number, errmsg: string | undefined): void => {

                if (errmsg === undefined) {
                    let e = new Event();
                    e.data = {
                        type: DBCommandType.SET,
                        object: object,
                        entry: entry,
                        keys: keys
                    };
                    this.emit("change", e);
                }
                if (done !== undefined) {
                    done(errmsg);
                }
            });

        }
    }
}