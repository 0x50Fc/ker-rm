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
        var responseType = HttpRequest.ResponseTypeString;
        var url = object.url;
        var method = object.method || "GET";
        if (object.responseType == "arraybuffer") {
            responseType = HttpRequest.ResponseTypeArrayBuffer;
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
        var req = new HttpRequest();
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
