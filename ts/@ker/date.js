var ker;
(function (ker) {
    function dateFormat(d, fmt) {
        if (fmt === void 0) { fmt = "yyyy-MM-dd hh:mm:ss"; }
        if (!(d instanceof Date)) {
            d = new Date(parseInt(d + ''));
        }
        var o = {
            "M+": d.getMonth() + 1,
            "d+": d.getDate(),
            "h+": d.getHours(),
            "m+": d.getMinutes(),
            "s+": d.getSeconds(),
            "q+": Math.floor((d.getMonth() + 3) / 3),
            "S": d.getMilliseconds() //毫秒 
        };
        fmt = fmt.replace(/(y+)/, d.getFullYear() + "");
        for (var k in o) {
            var v = o[k] + '';
            if (v.length < 2) {
                v = '0' + v;
            }
            fmt = fmt.replace(new RegExp("(" + k + ")"), v);
        }
        return fmt;
    }
    ker.dateFormat = dateFormat;
})(ker || (ker = {}));
