

interface WebKitMessageHandler {
    postMessage(data: any): void;
}

interface WebKitMessageHandlers {
    ker?: WebKitMessageHandler
}

interface WebKit {
    readonly messageHandlers?: WebKitMessageHandlers;
}

interface WebKitWindow {
    readonly webkit?: WebKit;
    readonly ker?:WebKitMessageHandler;
}

let readys:(()=>void)[] = [];

window.addEventListener("ker",function(){
    let w: WebKitWindow = window as WebKitWindow;
    while(readys.length > 0){
        let func = readys.shift();
        if(func !== undefined) {
            func();
        }
    }
});

export function postMessage(data: any): void {
    let w: WebKitWindow = window as WebKitWindow;
    if (w.webkit && w.webkit.messageHandlers && w.webkit.messageHandlers.ker) {
        w.webkit.messageHandlers.ker.postMessage(data);
    } else if(w.ker) {
        w.ker.postMessage(JSON.stringify(data));
    } else {
        ready(():void=>{
            w.ker!.postMessage(JSON.stringify(data));
        });
    }
}

export function ready(func:()=>void):void {
    let w: WebKitWindow = window as WebKitWindow;
    if (w.webkit && w.webkit.messageHandlers && w.webkit.messageHandlers.ker) {
        func();
    } else if(w.ker) {
        func();
    } else {
        readys.push(func);
    }
}
