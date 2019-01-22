
(function () {

	var modules = {};

	var join = function(base,path) {
		var vs = base.split("/");
		var items = path.split("/");
		while(items.length > 0) {
			var i = items.shift();
			if(i == '.') {
				
			}
		}
	};

	require = function (path) {
		var m = modules[path];
		if (m === undefined) {
			m = { exports: {} };
			try {
				var code = app.getTextContent(path);
				var fn = typeof compile === 'undefined' ? eval("(function(module,exports,__dirname){" + code + "})") : compile("(function(module,exports){" + code + "})", path)();
				if (typeof fn == 'function') {
					fn(m, m.exports);
				}
				print("[REQUIRE] " + path);
			} catch (e) {
				print("[ERQUIRE] [ERROR] " + e.fileName + "(" + e.lineNumber + "): " + e.stack);
			}
			modules[path] = m;
		}
		return m.exports;
	};

})();
