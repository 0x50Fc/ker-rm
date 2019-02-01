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
        Audio.startSession(Audio.PlayAndRecord, function (errmsg) {
            if (errmsg) {
                if (object.fail !== undefined) {
                    object.fail(errmsg);
                }
                if (object.complete !== undefined) {
                    object.complete();
                }
            }
            else {
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
                            duration: queue.duration
                        });
                    }
                    if (object.complete !== undefined) {
                        object.complete();
                    }
                    recycle();
                });
                queue.start();
            }
        });
    }
    ker.startRecord = startRecord;
    function stopRecord() {
        if (queue !== undefined) {
            queue.stop();
        }
    }
    ker.stopRecord = stopRecord;
    var playQueue;
    var playInput;
    function playRecycle() {
        if (playQueue !== undefined) {
            playQueue.off();
            playQueue.stop();
            playQueue = undefined;
        }
        if (playInput !== undefined) {
            playInput.close();
            playInput = undefined;
        }
    }
    function playVoice(object) {
        playRecycle();
        Audio.startSession(Audio.Playback, function (errmsg) {
            if (errmsg) {
                if (object.fail !== undefined) {
                    object.fail(errmsg);
                }
                if (object.complete !== undefined) {
                    object.complete();
                }
            }
            else {
                var input = object.file.openInputStream();
                if (input === undefined) {
                    if (object.fail !== undefined) {
                        object.fail("未找到文件");
                    }
                    if (object.complete !== undefined) {
                        object.complete();
                    }
                    return;
                }
                var buffer = new BufferInputStream(input, 2048);
                playInput = new SpeexFileInputStream(buffer);
                if (playInput === undefined || playInput.codec === undefined) {
                    playRecycle();
                    if (object.fail !== undefined) {
                        object.fail("无法识别文件");
                    }
                    if (object.complete !== undefined) {
                        object.complete();
                    }
                    return;
                }
                playQueue = new AudioQueueOutput(playInput.codec, playInput);
                playQueue.on("error", function (e) {
                    if (object.fail !== undefined) {
                        object.fail(e.data.errmsg);
                    }
                    if (object.complete !== undefined) {
                        object.complete();
                    }
                    playRecycle();
                });
                playQueue.on("done", function (e) {
                    if (object.success !== undefined) {
                        object.success();
                    }
                    if (object.complete !== undefined) {
                        object.complete();
                    }
                    playRecycle();
                });
                playQueue.start();
            }
        });
    }
    ker.playVoice = playVoice;
    function stopVoice(object) {
        playRecycle();
        if (object.success !== undefined) {
            object.success();
        }
        if (object.complete !== undefined) {
            object.complete();
        }
    }
    ker.stopVoice = stopVoice;
})(ker || (ker = {}));
