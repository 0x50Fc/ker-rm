
namespace ker {

    let queue: AudioQueueInput | undefined
    let output: SpeexFileOutputStream | undefined;
    let file: File | undefined;

    function recycle(): void {

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

    export interface KerAudioStartRecordRes {
        readonly tempFile: File
        readonly duration: number
    }

    export interface KerAudioStartRecordObject {
        success?: (res: KerAudioStartRecordRes) => void
        fail?: (errmsg?: string) => void
        complete?: () => void
    }

    export function startRecord(object: KerAudioStartRecordObject): void {

        recycle();

        Audio.startSession(Audio.Record, (errmsg?: string): void => {

            if (errmsg) {
                if (object.fail !== undefined) {
                    object.fail(errmsg);
                }
                if (object.complete !== undefined) {
                    object.complete();
                }
            } else {

                file = app.openTempFile("ker_startRecord_", ".spx");

                let input = file.openOutputStream();
                let buffer = new BufferOutputStream(input, 2048);

                output = new SpeexFileOutputStream(buffer);

                queue = new AudioQueueInput(output.codec, output);

                queue.on("error", function (e: Event) {
                    if (object.fail !== undefined) {
                        object.fail(e.data.errmsg);
                    }
                    if (object.complete !== undefined) {
                        object.complete();
                    }
                    recycle();
                });

                queue.on("done", function (e: Event) {
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

    export function stopRecord(): void {
        if (queue !== undefined) {
            queue.stop()
        }
    }

    export interface AudioPlayVoiceObject {
        file: File
        success?: () => void
        fail?: (errmsg?: string) => void
        complete?: () => void
    }

    export interface AudioStopVoiceObject {
        success?: () => void
        fail?: (errmsg?: string) => void
        complete?: () => void
    }

    let playQueue: AudioQueueOutput | undefined
    let playInput: SpeexFileInputStream | undefined;

    function playRecycle(): void {

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

    export function playVoice(object: AudioPlayVoiceObject): void {

        playRecycle();

        Audio.startSession(Audio.Ambient, (errmsg?: string): void => {

            if (errmsg) {
                if (object.fail !== undefined) {
                    object.fail(errmsg);
                }
                if (object.complete !== undefined) {
                    object.complete();
                }
            } else {

                let input = object.file.openInputStream();

                if(input === undefined) {
                    if (object.fail !== undefined) {
                        object.fail("未找到文件");
                    }
                    if (object.complete !== undefined) {
                        object.complete();
                    }
                    return;
                }

                let buffer = new BufferInputStream(input, 2048);

                playInput = new SpeexFileInputStream(buffer);

                if(playInput === undefined || playInput.codec === undefined) {
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

                playQueue.on("error", function (e: Event) {
                    if (object.fail !== undefined) {
                        object.fail(e.data.errmsg);
                    }
                    if (object.complete !== undefined) {
                        object.complete();
                    }
                    playRecycle();
                });

                playQueue.on("done", function (e: Event) {
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

    export function stopVoice(object: AudioStopVoiceObject): void {
        playRecycle();
        if (object.success !== undefined) {
            object.success();
        }
        if (object.complete !== undefined) {
            object.complete();
        }
    }
}
