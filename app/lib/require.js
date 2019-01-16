
(function(){
	var modules = {};
	require = function(path) {
		var m = modules[path];
		if(m === undefined) {
			m = { exports : {} };
			try {
				var code = app.getTextContent(path);
                var fn = typeof compile === 'undefined' ? eval("(function(module,exports){" + code + "})") : compile("(function(module,exports){" + code + "})",path)();
                if(typeof fn == 'function') {
                    fn(m,m.exports);
                }
				print("[REQUIRE] " + path);
			} catch(e) {
                print("[ERQUIRE] [ERROR] " + e.fileName + "(" + e.lineNumber + "): " + e.stack);
			}
			modules[path] = m;
		}
		return m.exports;
	};
})();
