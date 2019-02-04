var ker;
(function (ker) {
    function maxlength(s, maxlength, tail) {
        if (tail === void 0) { tail = "..."; }
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
    ker.maxlength = maxlength;
})(ker || (ker = {}));
