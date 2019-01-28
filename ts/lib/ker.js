

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
    function View(document, object, cb) {
        var pageViewContext = [];
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
        cb(v_Element, function (func, keys) {
            return new ker.Evaluate(func, keys);
        });
        pageViewContext.pop();
    }
    ker.View = View;
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
        ker.View(document, object, function (V, E) {
            app.exec(basename + "_" + object.view + ".js", {
                element: element,
                data: data,
                V: V,
                E: E
            });
        });
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
var ker;
(function (ker) {
    var queue;
    var output;
    var uri = "ker-tmp:///ker_startRecord_" + mktemp("XXXXXXXX") + ".spx";
    function recycle() {
        if (queue !== undefined) {
            queue.off();
            queue.stop();
            queue = undefined;
        }
        if (output !== undefined) {
            output.close();
            output = undefined;
        }
        app.removeURI(uri);
    }
    function startRecord(object) {
        recycle();
        var input = app.openOutputStream(uri);
        var buffer = new BufferOutputStream(input, 2048);
        output = new SpeexFileOutputStream(buffer);
        queue = new AudioQueueInput(output.codec, output);
        queue.on("error", function (e) {
            if (object.fail !== undefined) {
                object.fail(e.data.errmsg);
            }
            if (object.complete !== undefined) {
                object.complete();
            }
            recycle();
        });
        queue.on("done", function (e) {
            if (object.success !== undefined) {
                object.success({
                    tempFilePath: uri
                });
            }
            if (object.complete !== undefined) {
                object.complete();
            }
            recycle();
        });
        queue.start();
    }
    ker.startRecord = startRecord;
    function stopRecord() {
        if (queue !== undefined) {
            queue.stop();
        }
    }
    ker.stopRecord = stopRecord;
})(ker || (ker = {}));
var ker;
(function (ker) {
    var RequestRes = /** @class */ (function () {
        function RequestRes() {
        }
        return RequestRes;
    }());
    ker.RequestRes = RequestRes;
    var RequestTask = /** @class */ (function () {
        function RequestTask(request) {
            var _this = this;
            this._request = request;
            request.on("response", function (event) {
                _this.onResponse(request.responseHeaders);
            });
        }
        RequestTask.prototype.onResponse = function (header) {
            if (this._onHeadersReceived !== undefined) {
                this._onHeadersReceived(header);
            }
        };
        RequestTask.prototype.onHeadersReceived = function (v) {
            this._onHeadersReceived = v;
        };
        RequestTask.prototype.offHeadersReceived = function (v) {
            if (v === undefined || v == this._onHeadersReceived) {
                this._onHeadersReceived = undefined;
            }
        };
        RequestTask.prototype.abort = function () {
            this._request.cancel();
        };
        return RequestTask;
    }());
    ker.RequestTask = RequestTask;
    function request(object) {
        var responseType = HTTPRequest.ResponseTypeString;
        var url = object.url;
        var method = object.method || "GET";
        if (object.responseType == "arraybuffer") {
            responseType = HTTPRequest.ResponseTypeArrayBuffer;
        }
        if (method == 'GET') {
            if (typeof object.data == 'object') {
                var vs = [];
                for (var key in object.data) {
                    var v = object.data[key];
                    vs.push(key + '=' + encodeURIComponent(v + ''));
                }
                if (vs.length > 0) {
                    if (url.endsWith("?")) {
                        url += vs.join("&");
                    }
                    else if (url.indexOf("?") >= 0) {
                        url += "&" + vs.join("&");
                    }
                    else {
                        url += "?" + vs.join("&");
                    }
                }
            }
        }
        var req = new HTTPRequest();
        var contentType;
        if (object.header) {
            for (var key in object.header) {
                req.setRequestHeader(key, object.header[key]);
                if (key.toLowerCase() == 'content-type') {
                    contentType = object.header[key];
                }
            }
        }
        if (contentType === undefined && method != 'GET') {
            contentType = 'application/x-www-form-urlencoded';
            req.setRequestHeader('Content-Type', contentType);
        }
        req.on("done", function (event) {
            if (object.success !== undefined) {
                var res = new RequestRes();
                if (object.responseType == "arraybuffer") {
                    res.data = req.responseArrayBuffer;
                }
                else if (object.dataType === undefined || object.dataType == 'json') {
                    try {
                        res.data = JSON.parse(req.responseText);
                    }
                    catch (e) {
                        if (object.fail !== undefined) {
                            object.fail(e + '');
                        }
                        if (object.complete !== undefined) {
                            object.complete();
                        }
                        return;
                    }
                }
                else {
                    res.data = req.responseText;
                }
                res.statusCode = req.statusCode;
                res.header = req.responseHeaders;
                object.success(res);
            }
            if (object.complete !== undefined) {
                object.complete();
            }
        });
        req.on("error", function (event) {
            if (object.fail !== undefined) {
                object.fail(event.data.errmsg);
            }
            if (object.complete !== undefined) {
                object.complete();
            }
        });
        req.open(method, url, responseType);
        if (method != 'GET') {
            if (object.data instanceof ArrayBuffer) {
                req.send(object.data);
            }
            else if (typeof object.data == 'string') {
                req.send(object.data);
            }
            else if (typeof object.data == 'object') {
                if (contentType == 'json') {
                    req.send(JSON.stringify(object.data));
                }
                else {
                    var vs = [];
                    for (var key in object.data) {
                        var v = object.data[key];
                        vs.push(key + '=' + encodeURIComponent(v + ''));
                    }
                    req.send(vs.join('&'));
                }
            }
            else {
                req.send();
            }
        }
        else {
            req.send();
        }
        return new RequestTask(req);
    }
    ker.request = request;
})(ker || (ker = {}));
var ker;
(function (ker) {
    var Dialog = /** @class */ (function () {
        function Dialog() {
            var _this = this;
            this._data = new ker.Data(global);
            this._view = app.createView("view");
            this._viewContext = new UIViewContext(app);
            this._viewContext.view = this._view;
            this._document = new Document();
            this._element = this._document.createElement("layout");
            this._document.rootElement = this._element;
            this._viewElement = this._document.createElement("view");
            this._element.append(this._viewElement);
            this._layouting = false;
            this._onLayout = function (event) {
                _this.setLayout();
            };
            screen.on("change", this._onLayout);
        }
        Dialog.prototype.recycle = function () {
            screen.off("change", this._onLayout);
            this._data.off([]);
            this._viewElement.recycleView();
            this._view.removeView();
        };
        Dialog.prototype.layout = function () {
            this._viewContext.setSize(screen.width, screen.height);
            this._viewContext.setUnit("px", screen.density, 0);
            this._element.setFrame(0, 0, screen.width, screen.height);
            this._viewContext.layout(this._element);
            this._viewContext.obtainView(this._viewElement);
            this._layouting = false;
        };
        Dialog.prototype.setLayout = function () {
            var _this = this;
            if (this._layouting) {
                return;
            }
            this._layouting = true;
            setTimeout(function () {
                _this.layout();
            }, 0);
        };
        Dialog.prototype.setData = function (data) {
            this._data.setData(data);
            this.setLayout();
        };
        Object.defineProperty(Dialog.prototype, "view", {
            get: function () {
                return this._view;
            },
            enumerable: true,
            configurable: true
        });
        Object.defineProperty(Dialog.prototype, "document", {
            get: function () {
                return this._document;
            },
            enumerable: true,
            configurable: true
        });
        Dialog.prototype.create = function (object, func) {
            var _this = this;
            ker.View(this._document, object, function (V, E) {
                func(_this._viewElement, _this._data, V, E);
            });
            this.setLayout();
        };
        Dialog.prototype.open = function (path, object) {
            var _this = this;
            ker.View(this._document, object, function (V, E) {
                app.exec(path, {
                    element: _this._viewElement,
                    data: _this._data,
                    V: V,
                    E: E
                });
            });
            this.setLayout();
        };
        Dialog.prototype.show = function () {
            this._view.showToScreen();
        };
        Dialog.prototype.hide = function () {
            this._view.removeView();
        };
        return Dialog;
    }());
    ker.Dialog = Dialog;
})(ker || (ker = {}));
var __extends = (this && this.__extends) || (function () {
    var extendStatics = function (d, b) {
        extendStatics = Object.setPrototypeOf ||
            ({ __proto__: [] } instanceof Array && function (d, b) { d.__proto__ = b; }) ||
            function (d, b) { for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p]; };
        return extendStatics(d, b);
    }
    return function (d, b) {
        extendStatics(d, b);
        function __() { this.constructor = d; }
        d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
    };
})();
var ker;
(function (ker) {
    var ToastView = /** @class */ (function (_super) {
        __extends(ToastView, _super);
        function ToastView() {
            return _super !== null && _super.apply(this, arguments) || this;
        }
        ToastView.prototype.create = function (object) {
            _super.prototype.create.call(this, object, function (element, data, V, E) {
                V(element, data, "body", {
                    'max-width': '400rpx',
                    'padding': '20rpx',
                    'background-color': 'rgba(0,0,0,0.75)',
                    'border-radius': '8rpx'
                }, function (element, data) {
                    V(element, data, "text", {
                        '#text': E(function (title) { return title; }, ['title']),
                        'font': '28rpx',
                        'color': '#fff'
                    }, function (element, data) { });
                });
            });
        };
        return ToastView;
    }(ker.Dialog));
    var audoId = 0;
    var viewSet = {};
    function showToast(object) {
        if (object.duration === undefined) {
            object.duration = 1500;
        }
        var id = (++audoId) + '';
        var view = new ToastView();
        view.create({});
        view.setData({
            title: object.title
        });
        view.show();
        viewSet[id] = view;
        setTimeout(function () {
            view.hide();
            view.recycle();
            delete viewSet[id];
        }, object.duration);
        if (object.success !== undefined) {
            object.success();
        }
        if (object.complete !== undefined) {
            object.complete();
        }
    }
    ker.showToast = showToast;
    function hideToast(object) {
        var view;
        var id = 0;
        for (var key in viewSet) {
            if (parseInt(key) > id) {
                id = parseInt(key);
                view = viewSet[key];
            }
        }
        if (view !== undefined) {
            view.hide();
            view.recycle();
            delete viewSet[id + ''];
        }
        if (object.success !== undefined) {
            object.success();
        }
        if (object.complete !== undefined) {
            object.complete();
        }
    }
    ker.hideToast = hideToast;
})(ker || (ker = {}));
global.ker = ker;
