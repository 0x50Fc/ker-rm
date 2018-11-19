

interface WebKitMessageHandler {
    postMessage(data: any): void;
}

interface WebKitMessageHandlers {
    kk?: WebKitMessageHandler
}

interface WebKit {
    readonly messageHandlers?: WebKitMessageHandlers;
}

interface WebKitWindow {
    readonly webkit?: WebKit;
}

export function postMessage(data: any): void {
    let w: WebKitWindow = window as WebKitWindow;

    if (w.webkit && w.webkit.messageHandlers && w.webkit.messageHandlers.kk) {
        w.webkit.messageHandlers.kk.postMessage(data);
    }
}
