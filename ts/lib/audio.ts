
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
        readonly tempFilePath: string
        readonly tempFile: File
    }

    export interface KerAudioStartRecordObject {
        success?: (res: KerAudioStartRecordRes) => void
        fail?: (errmsg?: string) => void
        complete?: () => void
    }

    export function startRecord(object: KerAudioStartRecordObject): void {

        recycle();

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

    export function stopRecord(): void {
        if (queue !== undefined) {
            queue.stop()
        }
    }
}
