
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
        private _request: HTTPRequest
        constructor(request: HTTPRequest) {
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


        let responseType = HTTPRequest.ResponseTypeString;
        let url = object.url;
        let method = object.method || "GET";

        if (object.responseType == "arraybuffer") {
            responseType = HTTPRequest.ResponseTypeArrayBuffer;
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

        console.info("[HTTP]", url);

        let req = new HTTPRequest();
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
                    let v = req.responseText;
                    try {
                        res.data = JSON.parse(v);
                    } catch (e) {
                        console.info("[HTTP] [JSON] [ERROR]", v);
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

    export interface DownloadFileRes {
        readonly tempFile: File
        readonly tempFilePath: string
        readonly statusCode: number
    }

    export class DownloadFileTask {
        private _onHeadersReceived?: (header: HeaderSet) => void
        private _onProgresUpdate?: (progress: number, totalBytesWritten: number, totalBytesExpectedToWrite: number) => void
        private _request: HTTPRequest
        constructor(request: HTTPRequest) {
            this._request = request;
            request.on("response", (event: Event): void => {
                this.onResponse(request.responseHeaders);
            });
            request.on("progress", (event: Event): void => {
                let data = event.data;
                let bytes = data.bytes as number;
                let total = data.total as number;
                this.onProgress(total > 0 ? parseInt((100 * bytes / total) + '') : 0, total, bytes);
            });
        }
        private onResponse(header: HeaderSet) {
            if (this._onHeadersReceived !== undefined) {
                this._onHeadersReceived(header);
            }
        }
        private onProgress(progress: number, totalBytesWritten: number, totalBytesExpectedToWrite: number) {
            if (this._onProgresUpdate !== undefined) {
                this._onProgresUpdate(progress, totalBytesWritten, totalBytesExpectedToWrite);
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
        onProgresUpdate(v: (progress: number, totalBytesWritten: number, totalBytesExpectedToWrite: number) => void): void {
            this._onProgresUpdate = v;
        }
        offProgresUpdate(v?: (progress: number, totalBytesWritten: number, totalBytesExpectedToWrite: number) => void): void {
            if (v === undefined || v == this._onProgresUpdate) {
                this._onProgresUpdate = undefined;
            }
        }
        abort(): void {
            this._request.cancel();
        }
    }

    export interface DownloadFileObject {
        /**
         * url	string		是	下载资源的 url	
         * header	Object		否	HTTP 请求的 Header，Header 中不能设置 Referer	
         * filePath	string		否	指定文件下载后存储的路径	1.8.0
         * success	function		否	接口调用成功的回调函数	
         * fail	function		否	接口调用失败的回调函数	
         * complete	function		否	接口调用结束的回调函数（调用成功、失败都会执行）
         */
        url: string
        header?: HeaderSet
        file?: File
        filePath?: string
        success?: (res: DownloadFileRes) => void
        fail?: (errmsg?: string) => void
        complete?: () => void
    }

    export function downloadFile(object: DownloadFileObject): DownloadFileTask {

        let url = object.url;

        console.info("[HTTP]", url);

        let req = new HTTPRequest();

        if (object.header) {
            for (let key in object.header) {
                req.setRequestHeader(key, object.header[key]);
            }
        }

        req.on("done", (event: Event): void => {

            let file = req.responseFile!;

            let success = (file: File): void => {
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
                file.move(object.file, (): void => {
                    success(object.file!);
                });
            } else {
                success(file);
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

        req.open("GET", url, HTTPRequest.ResponseTypeFile);

        req.send();

        return new DownloadFileTask(req);
    }

    export interface UploadFileRes {
        readonly data: string
        readonly statusCode: number
    }

    export class UploadFileTask {
        private _onHeadersReceived?: (header: HeaderSet) => void
        private _onProgresUpdate?: (progress: number, totalBytesWritten: number, totalBytesExpectedToWrite: number) => void
        private _request: HTTPRequest
        constructor(request: HTTPRequest) {
            this._request = request;
            request.on("response", (event: Event): void => {
                this.onResponse(request.responseHeaders);
            });
            request.on("progress", (event: Event): void => {
                let data = event.data;
                let bytes = data.bytes as number;
                let total = data.total as number;
                this.onProgress(total > 0 ? parseInt((100 * bytes / total) + '') : 0, total, bytes);
            });
        }
        private onResponse(header: HeaderSet) {
            if (this._onHeadersReceived !== undefined) {
                this._onHeadersReceived(header);
            }
        }
        private onProgress(progress: number, totalBytesWritten: number, totalBytesExpectedToWrite: number) {
            if (this._onProgresUpdate !== undefined) {
                this._onProgresUpdate(progress, totalBytesWritten, totalBytesExpectedToWrite);
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
        onProgresUpdate(v: (progress: number, totalBytesWritten: number, totalBytesExpectedToWrite: number) => void): void {
            this._onProgresUpdate = v;
        }
        offProgresUpdate(v?: (progress: number, totalBytesWritten: number, totalBytesExpectedToWrite: number) => void): void {
            if (v === undefined || v == this._onProgresUpdate) {
                this._onProgresUpdate = undefined;
            }
        }
        abort(): void {
            this._request.cancel();
        }
    }

    export interface UploadFileFormData {
        [key: string]: string
    }

    export interface UploadFileObject {
        /**
        url	string		是	开发者服务器地址
        filePath	string		是	要上传文件资源的路径
        name	string		是	文件对应的 key，开发者在服务端可以通过这个 key 获取文件的二进制内容
        header	Object		否	HTTP 请求 Header，Header 中不能设置 Referer
        formData	Object		否	HTTP 请求中其他额外的 form data
        success	function		否	接口调用成功的回调函数
        fail	function		否	接口调用失败的回调函数
        complete	function		否	接口调用结束的回调函数（调用成功、失败都会执行）
         */
        url: string
        name: string
        file?: File
        filePath?: string
        header?: HeaderSet
        formData?: UploadFileFormData
        success?: (res: UploadFileRes) => void
        fail?: (errmsg?: string) => void
        complete?: () => void
    }

    export function uploadFile(object: UploadFileObject): UploadFileTask {

        let url = object.url;

        console.info("[HTTP]", url);

        let req = new HTTPRequest();

        if (object.header) {
            for (let key in object.header) {
                req.setRequestHeader(key, object.header[key]);
            }
        }

        req.on("done", (event: Event): void => {

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

        req.on("error", (event: Event): void => {
            if (object.fail !== undefined) {
                object.fail(event.data.errmsg);
            }
            if (object.complete !== undefined) {
                object.complete();
            }
        });

        req.open("POST", url, HTTPRequest.ResponseTypeString);

        let form = new FormData();

        if (object.formData !== undefined) {
            for (let key in object.formData) {
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
}
