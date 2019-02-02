
/**
 *  v2.4.4 (2019.01.05)
 *  https://developers.weixin.qq.com/miniprogram/dev/framework/release/v2.html
 * */

export interface WXRequestTask extends ker.RequestTask {

}


export interface WXRequestObject extends ker.RequestObject {

}

export function request(object: WXRequestObject): WXRequestTask {
    return ker.request(object)
}


export interface WXUploadFileRes extends ker.UploadFileRes {
}

export interface WXUploadFileTask extends ker.UploadFileTask {
}

export interface WXUploadFileObject extends ker.UploadFileObject {
}

export function uploadFile(object: WXUploadFileObject): WXUploadFileTask {
    return ker.uploadFile(object);
}

export interface WXDownloadFileRes extends ker.DownloadFileRes {
}

export interface WXDownloadFileTask extends ker.DownloadFileTask {
}

export interface WXDownloadFileObject extends ker.DownloadFileObject {
}

export function downloadFile(object: WXDownloadFileObject): WXDownloadFileTask {
    return ker.downloadFile(object)
}

export function reportAnalytics(name: string, data?: any): void {

}

export interface WXSaveFileObject {
    readonly tempFilePath: string
    readonly success?: (res: WXSaveFileRes) => void
    readonly fail?: (errmsg?: string) => void
    readonly complete?: () => void
}

export interface WXSaveFileRes {
    readonly savedFilePath: string
}

export function saveFile(object: WXSaveFileObject): void {
    let file = app.openFileURI(object.tempFilePath);
    if (file === undefined) {
        if (object.fail !== undefined) {
            object.fail("未找到文件");
        }
        if (object.complete !== undefined) {
            object.complete();
        }
    } else {
        let i = object.tempFilePath.indexOf("://");
        let path = object.tempFilePath.substr(i + 3);
        let toFile = app.openFile(UIApp.kDataDirectory, "file" + path);
        file.move(toFile, (): void => {
            if (object.success !== undefined) {
                object.success({
                    savedFilePath: toFile.name
                });
            }
            if (object.complete !== undefined) {
                object.complete();
            }
        });
    }
}

export interface WXRemoveSavedFileObject {
    readonly filePath: string
    readonly success?: (res: any) => void
    readonly fail?: (errmsg?: string) => void
    readonly complete?: () => void
}

export function removeSavedFile(object: WXRemoveSavedFileObject): void {
    let file = app.openFileURI(object.filePath);
    if (file === undefined) {
        if (object.fail !== undefined) {
            object.fail("未找到文件");
        }
        if (object.complete !== undefined) {
            object.complete();
        }
    } else {
        file.remove((): void => {
            if (object.success !== undefined) {
                object.success({});
            }
            if (object.complete !== undefined) {
                object.complete();
            }
        });
    }
}

export interface APIInterface {
    request(object: WXRequestObject): WXRequestTask
    downloadFile(object: WXDownloadFileObject): WXDownloadFileTask
    uploadFile(object: WXUploadFileObject): WXUploadFileTask
    reportAnalytics(name: string, data?: any): void
    saveFile(object: WXSaveFileObject): void
    removeSavedFile(object: WXRemoveSavedFileObject): void
}

