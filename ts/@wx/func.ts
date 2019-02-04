
export function dirname(path: string): string {
    if (typeof path == 'string') {
        var i = path.lastIndexOf("/");
        if (i >= 0) {
            return path.substr(0, i);
        }
    }
    return path;
}

export function relativePath(path: string, basePath: string): string {
    var vs = path.split('/');
    var bs = basePath.split('/');

    while (vs.length) {
        if (vs[0] == '..') {
            bs.pop();
            vs.shift();
        } else if (vs[0] == '.') {
            vs.shift();
        } else {
            break;
        }
    }

    return bs.concat(vs).join('/');
}

