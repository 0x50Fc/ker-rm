var ker;
(function (ker) {
    var queue;
    var output;
    var file;
    function recycle() {
        if (queue !== undefined) {
            queue.off();
            queue.stop();
            queue = undefined;
        }
        if (output !== undefined) {
            output.close();
            output = undefined;
        }
        if (file !== undefined) {
            file.remove();
            file = undefined;
        }
    }
    function startRecord(object) {
        recycle();
        file = app.openTempFile("ker_startRecord_", ".spx");
        var input = file.openOutputStream();
        var buffer = new BufferOutputStream(input, 2048);
        output = new SpeexFileOutputStream(buffer);
        queue = new AudioQueueInput(output.codec, output);
        queue.on("error", function (e) {
            if (object.fail !== undefined) {
                object.fail(e.data.errmsg);
            }
            if (object.complete !== undefined) {
                object.complete();
            }
            recycle();
        });
        queue.on("done", function (e) {
            if (object.success !== undefined) {
                object.success({
                    tempFile: file,
                    tempFilePath: file.name
                });
            }
            if (object.complete !== undefined) {
                object.complete();
            }
            recycle();
        });
        queue.start();
    }
    ker.startRecord = startRecord;
    function stopRecord() {
        if (queue !== undefined) {
            queue.stop();
        }
    }
    ker.stopRecord = stopRecord;
})(ker || (ker = {}));
