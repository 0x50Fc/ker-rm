
namespace ker {

    export function maxlength(s: string, maxlength: number, tail: string="..."): string {
        var length = 0;
        for (var i = 0; i < s.length; i++) {
            var c = s.charCodeAt(i);
            var len = 1;
            if (c > 0x0ff) {
                len = 2;
            }
            if (length + len > maxlength) {
                return s.substr(0, i) + tail;
            }
            length += len;
        }
        return s;
    }
}
