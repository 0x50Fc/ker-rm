
interface TWindow {
    __basePath?: string
}

export function resolveURI(uri: string, basePath: string | undefined): string {
    if (uri.indexOf('://') < 0) {
        if (uri.startsWith("/")) {
            return 'file://' + uri;
        }
        if (basePath !== undefined) {
            return basePath + '/' + uri;
        }
    }
    return uri;
}
