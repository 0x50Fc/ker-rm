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
                            elements_1.push(e);
                            datas_1.push(d);
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
                        e.recycle();
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
                    e.recycle();
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
