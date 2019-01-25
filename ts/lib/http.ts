
namespace ker {

    export class RequestRes {
        data: any
        statusCode: number
        header: HeaderSet
    }

    export interface RequestObject {
        url: string
        data?: any
        header?: HeaderSet
        method?: string
        dataType?: string
        responseType?: string
        success?: (res: RequestRes) => void
        fail?: (errmsg?: string) => void
        complete?: () => void
    }

    export class RequestTask {
        private _onHeadersReceived?: (header: HeaderSet) => void
        private _request: HttpRequest
        constructor(request: HttpRequest) {
            this._request = request;
            request.on("response", (event: Event): void => {
                this.onResponse(request.responseHeaders);
            });
        }
        private onResponse(header: HeaderSet) {
            if (this._onHeadersReceived !== undefined) {
                this._onHeadersReceived(header);
            }
        }
        onHeadersReceived(v: (header: HeaderSet) => void): void {
            this._onHeadersReceived = v;
        }
        offHeadersReceived(v?: (header: HeaderSet) => void): void {
            if (v === undefined || v == this._onHeadersReceived) {
                this._onHeadersReceived = undefined;
            }
        }
        abort(): void {
            this._request.cancel();
        }
    }

    export function request(object: RequestObject): RequestTask {


        let responseType = HttpRequest.ResponseTypeString;
        let url = object.url;
        let method = object.method || "GET";

        if (object.responseType == "arraybuffer") {
            responseType = HttpRequest.ResponseTypeArrayBuffer;
        }

        if (method == 'GET') {
            if (typeof object.data == 'object') {
                let vs: string[] = [];

                for (let key in object.data) {
                    let v = object.data[key];
                    vs.push(key + '=' + encodeURIComponent(v + ''));
                }

                if (vs.length > 0) {
                    if (url.endsWith("?")) {
                        url += vs.join("&");
                    } else if (url.indexOf("?") >= 0) {
                        url += "&" + vs.join("&");
                    } else {
                        url += "?" + vs.join("&");
                    }
                }
            }
        }

        let req = new HttpRequest();
        let contentType: string | undefined;

        if (object.header) {
            for (let key in object.header) {
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

        req.on("done", (event: Event): void => {

            if (object.success !== undefined) {
                let res = new RequestRes();
                if (object.responseType == "arraybuffer") {
                    res.data = req.responseArrayBuffer
                } else if (object.dataType === undefined || object.dataType == 'json') {
                    try {
                        res.data = JSON.parse(req.responseText);
                    } catch (e) {
                        if (object.fail !== undefined) {
                            object.fail(e + '');
                        }
                        if (object.complete !== undefined) {
                            object.complete();
                        }
                        return;
                    }
                } else {
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

        req.on("error", (event: Event): void => {
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
            } else if (typeof object.data == 'string') {
                req.send(object.data);
            } else if (typeof object.data == 'object') {
                if (contentType == 'json') {
                    req.send(JSON.stringify(object.data));
                } else {
                    let vs: string[] = [];
                    for (let key in object.data) {
                        let v = object.data[key];
                        vs.push(key + '=' + encodeURIComponent(v + ''));
                    }
                    req.send(vs.join('&'));
                }
            } else {
                req.send();
            }

        } else {
            req.send();
        }

        return new RequestTask(req);
    }

}
