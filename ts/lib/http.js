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
        console.info("[HTTP]", url);
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
                    var v = req.responseText;
                    try {
                        res.data = JSON.parse(v);
                    }
                    catch (e) {
                        console.info("[HTTP] [JSON] [ERROR]", v);
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
    var DownloadFileTask = /** @class */ (function () {
        function DownloadFileTask(request) {
            var _this = this;
            this._request = request;
            request.on("response", function (event) {
                _this.onResponse(request.responseHeaders);
            });
            request.on("progress", function (event) {
                var data = event.data;
                var bytes = data.bytes;
                var total = data.total;
                _this.onProgress(total > 0 ? parseInt((100 * bytes / total) + '') : 0, total, bytes);
            });
        }
        DownloadFileTask.prototype.onResponse = function (header) {
            if (this._onHeadersReceived !== undefined) {
                this._onHeadersReceived(header);
            }
        };
        DownloadFileTask.prototype.onProgress = function (progress, totalBytesWritten, totalBytesExpectedToWrite) {
            if (this._onProgresUpdate !== undefined) {
                this._onProgresUpdate(progress, totalBytesWritten, totalBytesExpectedToWrite);
            }
        };
        DownloadFileTask.prototype.onHeadersReceived = function (v) {
            this._onHeadersReceived = v;
        };
        DownloadFileTask.prototype.offHeadersReceived = function (v) {
            if (v === undefined || v == this._onHeadersReceived) {
                this._onHeadersReceived = undefined;
            }
        };
        DownloadFileTask.prototype.onProgresUpdate = function (v) {
            this._onProgresUpdate = v;
        };
        DownloadFileTask.prototype.offProgresUpdate = function (v) {
            if (v === undefined || v == this._onProgresUpdate) {
                this._onProgresUpdate = undefined;
            }
        };
        DownloadFileTask.prototype.abort = function () {
            this._request.cancel();
        };
        return DownloadFileTask;
    }());
    ker.DownloadFileTask = DownloadFileTask;
    function downloadFile(object) {
        var url = object.url;
        console.info("[HTTP]", url);
        var req = new HTTPRequest();
        if (object.header) {
            for (var key in object.header) {
                req.setRequestHeader(key, object.header[key]);
            }
        }
        req.on("done", function (event) {
            var file = req.responseFile;
            var success = function (file) {
                if (object.success !== undefined) {
                    object.success({
                        tempFile: file,
                        tempFilePath: file.name,
                        statusCode: req.statusCode
                    });
                }
                if (object.complete !== undefined) {
                    object.complete();
                }
            };
            if (object.file === undefined && object.filePath !== undefined) {
                object.file = app.openFileURI(object.filePath);
            }
            if (object.file !== undefined) {
                file.move(object.file, function () {
                    success(object.file);
                });
            }
            else {
                success(file);
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
        req.open("GET", url, HTTPRequest.ResponseTypeFile);
        req.send();
        return new DownloadFileTask(req);
    }
    ker.downloadFile = downloadFile;
    var UploadFileTask = /** @class */ (function () {
        function UploadFileTask(request) {
            var _this = this;
            this._request = request;
            request.on("response", function (event) {
                _this.onResponse(request.responseHeaders);
            });
            request.on("progress", function (event) {
                var data = event.data;
                var bytes = data.bytes;
                var total = data.total;
                _this.onProgress(total > 0 ? parseInt((100 * bytes / total) + '') : 0, total, bytes);
            });
        }
        UploadFileTask.prototype.onResponse = function (header) {
            if (this._onHeadersReceived !== undefined) {
                this._onHeadersReceived(header);
            }
        };
        UploadFileTask.prototype.onProgress = function (progress, totalBytesWritten, totalBytesExpectedToWrite) {
            if (this._onProgresUpdate !== undefined) {
                this._onProgresUpdate(progress, totalBytesWritten, totalBytesExpectedToWrite);
            }
        };
        UploadFileTask.prototype.onHeadersReceived = function (v) {
            this._onHeadersReceived = v;
        };
        UploadFileTask.prototype.offHeadersReceived = function (v) {
            if (v === undefined || v == this._onHeadersReceived) {
                this._onHeadersReceived = undefined;
            }
        };
        UploadFileTask.prototype.onProgresUpdate = function (v) {
            this._onProgresUpdate = v;
        };
        UploadFileTask.prototype.offProgresUpdate = function (v) {
            if (v === undefined || v == this._onProgresUpdate) {
                this._onProgresUpdate = undefined;
            }
        };
        UploadFileTask.prototype.abort = function () {
            this._request.cancel();
        };
        return UploadFileTask;
    }());
    ker.UploadFileTask = UploadFileTask;
    function uploadFile(object) {
        var url = object.url;
        console.info("[HTTP]", url);
        var req = new HTTPRequest();
        if (object.header) {
            for (var key in object.header) {
                req.setRequestHeader(key, object.header[key]);
            }
        }
        req.on("done", function (event) {
            if (object.success !== undefined) {
                object.success({
                    data: req.responseText,
                    statusCode: req.statusCode
                });
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
        req.open("POST", url, HTTPRequest.ResponseTypeString);
        var form = new FormData();
        if (object.formData !== undefined) {
            for (var key in object.formData) {
                form.append(key, object.formData[key]);
            }
        }
        if (object.file === undefined && object.filePath !== undefined) {
            object.file = app.openFileURI(object.filePath);
        }
        if (object.file !== undefined) {
            form.append(object.name, object.file);
        }
        req.send(form);
        return new UploadFileTask(req);
    }
    ker.uploadFile = uploadFile;
})(ker || (ker = {}));
