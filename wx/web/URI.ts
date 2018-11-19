
interface TWindow {
    __basePath?:string
}

export function resolveURI(uri:string):string {
    if(uri.indexOf('://') < 0) {
        let v = (window as TWindow).__basePath;
        if(v !== undefined) {
            return v + '/' + uri;
        }
    }
    return uri;
}
