
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
        let input = app.openInputStream(uri)!
        let buffer = new BufferOutputStream(input);

        output = new SpeexFileOutputStream(buffer);

        queue = new AudioQueueInput(output.codec,output);

        queue.on("error",function(e:Event){
            if(object.fail !== undefined) {
                object.fail(e.data.errmsg);
            }
            if(object.complete !== undefined) {
                object.complete();
            }
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
            object = undefined;
        });
    }

    export function stopRecord(): void {
        if(queue !== undefined) {
            queue.stop()
        }
    }
}
