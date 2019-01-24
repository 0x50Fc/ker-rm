
namespace ker {

    let queue:AudioQueueInput|undefined
    let output:SpeexFileOutputStream|undefined;

    function recycle():void {
        if(queue !== undefined) {
            queue.off();
            queue.stop();
            queue = undefined;
        }

        if(output !== undefined) {
            output.close();
            output = undefined;
        }
    }
    
    export interface KerAudioStartRecordRes {
        readonly tempFilePath: string
    }

    export interface KerAudioStartRecordObject {
        success?: (res: KerAudioStartRecordRes) => void
        fail?: (errmsg?: string) => void
        complete?: () => void
    }

    export function startRecord(object: KerAudioStartRecordObject): void {
        
        recycle();

        let uri = "ker-tmp:///ker_Audio_startRecord.spx";
        let input = app.openOutputStream(uri)!
        let buffer = new BufferOutputStream(input,2048);

        output = new SpeexFileOutputStream(buffer);

        queue = new AudioQueueInput(output.codec,output);

        queue.on("error",function(e:Event){
            if(object.fail !== undefined) {
                object.fail(e.data.errmsg);
            }
            if(object.complete !== undefined) {
                object.complete();
            }
            recycle();
        });

        queue.on("done",function(e:Event){
            if(object.success !== undefined) {
                object.success({
                    tempFilePath : uri
                });
            }
            if(object.complete !== undefined) {
                object.complete();
            }
            recycle();
        });

        queue.start();

    }

    export function stopRecord(): void {
        if(queue !== undefined) {
            queue.stop()
        }
    }
}
