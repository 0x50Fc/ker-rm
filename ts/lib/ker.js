

(function () {
	
	var modules = {};

	require = function (path) {
		var m = modules[path];
		if (m === undefined) {
			m = { exports: {} };
			try {
				var code = app.getTextContent(path);
				var fn = typeof compile === 'undefined' ? eval("(function(module,exports){" + code + "})") : compile("(function(module,exports){" + code + "})", path)();
				if (typeof fn == 'function') {
					fn(m, m.exports);
				}
				print("[REQUIRE] " + path);
			} catch (e) {
				print("[ERQUIRE] [ERROR] " + e.fileName + "(" + e.lineNumber + "): " + e.stack);
			}
			modules[path] = m;
		}
		return m.exports;
    };
    
})();

(function () {
	
	function parseArguments(vs, args) {
		for (var i = 0; i < args.length; i++) {
			var v = args[i];
			switch (typeof v) {
				case 'object':
					vs.push(JSON.stringify(v, undefined, 4));
					break;
				default:
					vs.push(v + '');
					break;
			}
		}
	}

	console = {
		log: function () {
			var vs = ["[LOG]"];
			parseArguments(vs, arguments);
			print(vs.join(" "));
		},
		error: function () {
			var vs = ["[ERROR]"];
			parseArguments(vs, arguments);
			print(vs.join(" "));
		},
		debug: function () {
			var vs = ["[DEBUG]"];
			parseArguments(vs, arguments);
			print(vs.join(" "));
		},
		info: function () {
			var vs = ["[INFO]"];
			parseArguments(vs, arguments);
			print(vs.join(" "));
		}
	};

})();
var ker;
(function (ker) {
    var Evaluate = /** @class */ (function () {
        function Evaluate(evaluateScript, keys) {
            this.evaluateScript = evaluateScript;
            this.keys = keys;
        }
        Evaluate.prototype.exec = function (object, global) {
            var vs = [];
            for (var _i = 0, _a = this.keys; _i < _a.length; _i++) {
                var key = _a[_i];
                var v = object[key];
                if (v === undefined) {
                    v = global[key];
                }
                vs.push(v);
            }
            try {
                return this.evaluateScript.apply(undefined, vs);
            }
            catch (e) {
                console.error("[Evaluate:exec]", e);
            }
        };
        return Evaluate;
    }());
    ker.Evaluate = Evaluate;
    var KeyCallback = /** @class */ (function () {
        function KeyCallback(func) {
            this.priority = 0;
            this.func = func;
        }
        KeyCallback.prototype.run = function (object, global, changedKeys) {
            var v;
            if (this.evaluate !== undefined) {
                v = this.evaluate.exec(object, global);
            }
            else if (this.keys !== undefined) {
                v = Data.get(object, this.keys);
            }
            this.func(v, changedKeys);
        };
        return KeyCallback;
    }());
    var IObject = /** @class */ (function () {
        function IObject() {
        }
        IObject.prototype.get = function (key) {
            return this[key];
        };
        IObject.prototype.set = function (key, value) {
            if (value === undefined) {
                delete this[key];
            }
            else {
                this[key] = value;
            }
        };
        return IObject;
    }());
    ker.IObject = IObject;
    var Data = /** @class */ (function () {
        function Data(global) {
            this._object = {};
            this._global = global;
            this._keyObserver = {};
            this._defaultObserver = [];
        }
        Object.defineProperty(Data.prototype, "global", {
            get: function () {
                return this._global;
            },
            enumerable: true,
            configurable: true
        });
        Object.defineProperty(Data.prototype, "object", {
            get: function () {
                return this._object;
            },
            set: function (object) {
                this._object = object;
            },
            enumerable: true,
            configurable: true
        });
        Data.prototype.setData = function (data, changed) {
            if (changed === void 0) { changed = true; }
            if (changed) {
                this.begin();
            }
            for (var key in data) {
                this._object[key] = data[key];
                if (this._keySet !== undefined) {
                    this._keySet[key] = true;
                }
            }
            if (changed) {
                this.commit();
            }
        };
        Data.prototype.begin = function () {
            this._keySet = {};
        };
        Data.prototype.commit = function () {
            var keys = this._keySet;
            this._keySet = undefined;
            if (keys !== undefined) {
                this.changeKeys(keys);
            }
        };
        Data.prototype.cancel = function () {
            this._keySet = undefined;
        };
        Data.prototype.changeKeys = function (keySet) {
            var cbs = [].concat(this._defaultObserver);
            if (keySet === undefined) {
                for (var key in this._keyObserver) {
                    var v = this._keyObserver[key];
                    if (v !== undefined) {
                        cbs = cbs.concat(v);
                    }
                }
            }
            else {
                for (var key in keySet) {
                    var v = this._keyObserver[key];
                    if (v !== undefined) {
                        cbs = cbs.concat(v);
                    }
                }
            }
            cbs.sort(function (a, b) {
                return a.priority - b.priority;
            });
            for (var _i = 0, cbs_1 = cbs; _i < cbs_1.length; _i++) {
                var cb = cbs_1[_i];
                cb.run(this._object, this._global, keySet);
            }
        };
        Data.prototype.on = function (keys, func, priority) {
            if (priority === void 0) { priority = 0; }
            var cb;
            if (keys instanceof Evaluate) {
                cb = new KeyCallback(func);
                cb.priority = priority;
                cb.keys = keys.keys;
                cb.evaluate = keys;
            }
            else {
                cb = new KeyCallback(func);
                cb.priority = priority;
                cb.keys = keys;
            }
            if (cb.keys.length == 0) {
                this._defaultObserver.push(cb);
            }
            else {
                for (var _i = 0, _a = cb.keys; _i < _a.length; _i++) {
                    var key = _a[_i];
                    var vs = this._keyObserver[key];
                    if (vs === undefined) {
                        this._keyObserver[key] = [cb];
                    }
                    else {
                        vs.push(cb);
                    }
                }
            }
        };
        Data.prototype.off = function (keys, func) {
            if (keys instanceof Evaluate) {
                keys = keys.keys;
            }
            if (keys.length == 0) {
                if (func === undefined) {
                    this._defaultObserver = [];
                }
                else {
                    var vs = [];
                    for (var _i = 0, _a = this._defaultObserver; _i < _a.length; _i++) {
                        var cb = _a[_i];
                        if (cb.func !== func) {
                            vs.push(cb);
                        }
                    }
                    this._defaultObserver = vs;
                }
            }
            else {
                for (var _b = 0, keys_1 = keys; _b < keys_1.length; _b++) {
                    var key = keys_1[_b];
                    if (func === undefined) {
                        delete this._keyObserver[key];
                    }
                    else {
                        var cbs = this._keyObserver[key];
                        if (cbs !== undefined) {
                            var vs = [];
                            for (var _c = 0, cbs_2 = cbs; _c < cbs_2.length; _c++) {
                                var cb = cbs_2[_c];
                                if (cb.func !== func) {
                                    vs.push(cb);
                                }
                            }
                            this._keyObserver[key] = vs;
                        }
                    }
                }
            }
        };
        Data.prototype.setParent = function (parent) {
            this.recycle();
            if (parent !== undefined) {
                this._parent = parent;
                var data_1 = this;
                this._onDataFunction = function (value, keySet) {
                    if (value !== undefined) {
                        if (keySet === undefined) {
                            data_1.changeKeys();
                        }
                        else {
                            data_1.begin();
                            for (var key_1 in keySet) {
                                data_1.set([key_1], value[key_1], false);
                            }
                            data_1.commit();
                        }
                    }
                };
                parent.on([], this._onDataFunction);
                var object = parent.object;
                for (var key in object) {
                    this._object[key] = object[key];
                }
            }
        };
        Data.prototype.recycle = function () {
            if (this._parent !== undefined) {
                this._parent.off([], this._onDataFunction);
                this._parent = undefined;
                this._onDataFunction = undefined;
            }
        };
        Data.prototype.get = function (keys) {
            return Data.get(this._object, keys);
        };
        Data.prototype.set = function (keys, value, changed) {
            if (changed === void 0) { changed = true; }
            if (changed) {
                this.begin();
            }
            Data.set(this._object, keys, value);
            if (this._keySet !== undefined && keys.length > 0) {
                this._keySet[keys[0]] = true;
            }
            if (changed) {
                this.commit();
            }
        };
        Data.get = function (object, keys, index) {
            if (index === void 0) { index = 0; }
            if (index < keys.length) {
                var key = keys[index];
                if (typeof object == 'object') {
                    if (object instanceof IObject) {
                        return Data.get(object.get(key), keys, index + 1);
                    }
                    return Data.get(object[key], keys, index + 1);
                }
            }
            else {
                return object;
            }
        };
        Data.set = function (object, keys, value, index) {
            if (index === void 0) { index = 0; }
            if (typeof object != 'object') {
                return;
            }
            if (index + 1 < keys.length) {
                var key = keys[index];
                var v;
                if (object instanceof IObject) {
                    v = object.get(key);
                }
                else {
                    v = object[key];
                }
                if (v === undefined) {
                    v = {};
                    if (object instanceof IObject) {
                        object.set(key, v);
                    }
                    else {
                        object[key] = v;
                    }
                }
                Data.set(v, keys, value, index + 1);
            }
            else if (index < keys.length) {
                var key = keys[index];
                if (object instanceof IObject) {
                    object.set(key, value);
                }
                else {
                    object[key] = value;
                }
            }
        };
        return Data;
    }());
    ker.Data = Data;
})(ker || (ker = {}));
var ker;
(function (ker) {
    function Page(object, page, setTimeout) {
        arguments.callee.caller;
        var basename = object.path;
        var i = basename.lastIndexOf(".");
        if (i >= 0) {
            basename = basename.substr(0, i);
        }
        if (object.view === undefined) {
            object.view = "view";
        }
        var document = new Document();
        var context = new UIViewContext(app);
        var layouting = false;
        var element = document.createElement("view");
        var data = new ker.Data(object);
        var pageViewContext = [];
        if (object.data !== undefined) {
            data.object = object.data;
        }
        document.rootElement = element;
        var layout = function () {
            if (page.view) {
                context.layout(element);
                context.obtainView(element);
            }
            layouting = false;
        };
        var setLayout = function () {
            if (layouting) {
                return;
            }
            layouting = true;
            setTimeout(layout, 0);
        };
        page.on("resize", function () {
            context.setSize(page.width, page.height);
            element.setFrame(0, 0, page.width, page.height);
            setLayout();
        });
        page.on("unload", function () {
            element.recycleView();
            if (object.onunload !== undefined) {
                object.onunload();
            }
        });
        page.on("ready", function () {
            context.view = page.view;
            element.setFrame(0, 0, page.width, page.height);
            layout();
        });
        page.on("data", function (e, name) {
            var v = e.data;
            if (typeof v == 'object') {
                data.setData(v);
                setLayout();
            }
        });
        document.on("layout", function () {
            setLayout();
        });
        function v_AttributeEvaluate(element, data, key, evaluate) {
            data.on(evaluate, function (value, changedKeys) {
                if (key == 'data') {
                    var e = new Event();
                    e.data = value;
                    element.emit(key, e);
                }
                else if (value !== undefined) {
                    element.set(key, value + '');
                }
            });
        }
        function v_AttributeEvent(element, name, key) {
            element.on(name, function (event, name) {
                var func = object[key];
                if (typeof func == 'function') {
                    func.call(object, event, name);
                }
            });
        }
        function v_AttributeSet(element, data, name, attrs) {
            for (var key in attrs) {
                var v = attrs[key];
                if (key.substr(0, 2) == 'on') {
                    if (typeof v == 'string') {
                        v_AttributeEvent(element, key.substr(2), v);
                    }
                }
                else if (v instanceof ker.Evaluate) {
                    v_AttributeEvaluate(element, data, key, v);
                }
                else {
                    element.set(key, v);
                }
            }
        }
        ;
        function v_createElement(name) {
            return document.createElement(name);
        }
        ;
        function v_For(element, data, name, attrs, children) {
            var v = attrs["kk:for"];
            if (v instanceof ker.Evaluate) {
                delete attrs["kk:for"];
                var indexKey_1 = attrs["kk:index"] || 'index';
                var itemKey_1 = attrs["kk:item"] || 'item';
                var elements_1 = [];
                var datas_1 = [];
                var before_1 = v_createElement("element");
                element.append(before_1);
                data.on(v, function (value, changedKeys) {
                    var index = 0;
                    var forItem = function (item) {
                        var e = index < elements_1.length ? elements_1[index] : undefined;
                        var d = index < datas_1.length ? datas_1[index] : undefined;
                        if (e === undefined) {
                            e = v_createElement(name);
                            before_1.before(e);
                            d = new ker.Data(data.global);
                            d.setParent(data);
                            v_AttributeSet(e, d, name, attrs);
                            pageViewContext.push([]);
                            children(e, d);
                            pageViewContext.pop();
                            d.set([indexKey_1], index, false);
                            d.set([itemKey_1], item, false);
                            d.changeKeys();
                        }
                        else {
                            d.begin();
                            d.set([indexKey_1], index, false);
                            d.set([itemKey_1], item, false);
                            d.commit();
                        }
                        index++;
                    };
                    if (value instanceof Array) {
                        for (var _i = 0, value_1 = value; _i < value_1.length; _i++) {
                            var v_1 = value_1[_i];
                            forItem(v_1);
                        }
                    }
                    while (index < elements_1.length) {
                        var e = elements_1.pop();
                        var d = datas_1.pop();
                        if (e instanceof UIViewElement) {
                            e.recycleView();
                        }
                        e.off();
                        e.remove();
                        d.recycle();
                    }
                });
            }
        }
        ;
        function v_IfElseIfElse(evaluate, element, data, name, attrs, children) {
            var scope = pageViewContext[pageViewContext.length - 1];
            var before = document.createElement("element");
            var e;
            var d;
            element.append(before);
            scope.push(function (enabled) {
                if (enabled && (evaluate === undefined || evaluate.exec(data.object, data.global))) {
                    if (e === undefined) {
                        e = v_createElement(name);
                        before.before(e);
                        d = new ker.Data(data.global);
                        d.setParent(data);
                        v_AttributeSet(e, d, name, attrs);
                        pageViewContext.push([]);
                        children(e, d);
                        pageViewContext.pop();
                    }
                    d.changeKeys();
                    return true;
                }
                else if (e !== undefined) {
                    if (e instanceof UIViewElement) {
                        e.recycleView();
                    }
                    e.off();
                    e.remove();
                    e = undefined;
                    d.recycle();
                    d = undefined;
                }
                return false;
            });
            if (evaluate !== undefined) {
                data.on(evaluate, function (value, changedKeys) {
                    var enabled = true;
                    for (var _i = 0, scope_1 = scope; _i < scope_1.length; _i++) {
                        var item = scope_1[_i];
                        if (item(enabled)) {
                            enabled = false;
                        }
                    }
                });
            }
        }
        ;
        function v_Element(element, data, name, attrs, children) {
            if (name == "body") {
                v_AttributeSet(element, data, name, attrs);
                pageViewContext.push([]);
                children(element, data);
                pageViewContext.pop();
            }
            else if (attrs["kk:for"]) {
                v_For(element, data, name, attrs, children);
            }
            else if (attrs["kk:if"]) {
                var v = attrs["kk:if"];
                if (v instanceof ker.Evaluate) {
                    delete attrs["kk:if"];
                    v_IfElseIfElse(v, element, data, name, attrs, children);
                }
            }
            else if (attrs["kk:elseif"]) {
                var v = attrs["kk:elseif"];
                if (v instanceof ker.Evaluate) {
                    delete attrs["kk:elseif"];
                    v_IfElseIfElse(v, element, data, name, attrs, children);
                }
            }
            else if (attrs["kk:else"]) {
                delete attrs["kk:else"];
                v_IfElseIfElse(undefined, element, data, name, attrs, children);
            }
            else {
                var e = v_createElement(name);
                element.append(e);
                v_AttributeSet(e, data, name, attrs);
                pageViewContext.push([]);
                children(e, data);
                pageViewContext.pop();
            }
        }
        ;
        pageViewContext.push([]);
        app.exec(basename + "_" + object.view + ".js", {
            element: element,
            data: data,
            V: v_Element,
            E: function (func, keys) {
                return new ker.Evaluate(func, keys);
            }
        });
        pageViewContext.pop();
        data.changeKeys();
        setLayout();
        console.info(element.toString());
        object.document = document;
        object.data = data.object;
        object.setData = function (object) {
            data.setData(object);
            setLayout();
        };
        if (object.onload !== undefined) {
            object.onload(document);
        }
    }
    ker.Page = Page;
})(ker || (ker = {}));

Page = ker.Page;
