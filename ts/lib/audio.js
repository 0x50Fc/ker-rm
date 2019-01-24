var ker;
(function (ker) {
    var queue;
    var output;
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
    }
    function startRecord(object) {
        recycle();
        var uri = "ker-tmp:///ker_Audio_startRecord.spx";
        var input = app.openInputStream(uri);
        var buffer = new BufferOutputStream(input);
        output = new SpeexFileOutputStream(buffer);
        queue = new AudioQueueInput(output.codec, output);
        queue.on("error", function (e) {
            if (object.fail !== undefined) {
                object.fail(e.data.errmsg);
            }
            if (object.complete !== undefined) {
                object.complete();
            }
        });
        queue.on("done", function (e) {
            if (object.success !== undefined) {
                object.success({
                    tempFilePath: uri
                });
            }
            if (object.complete !== undefined) {
                object.complete();
            }
            object = undefined;
        });
    }
    ker.startRecord = startRecord;
    function stopRecord() {
        if (queue !== undefined) {
            queue.stop();
        }
    }
    ker.stopRecord = stopRecord;
})(ker || (ker = {}));
