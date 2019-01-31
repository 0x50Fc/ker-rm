
declare class Event {
    constructor(target?: any)
    data: any
    returnValue: any
    readonly target: any
}

type EventFunction = (event: Event, name: string) => void

declare class EventEmitter {
    on(name: string, func: EventFunction): void
    off(name?: string, func?: EventFunction): void
    has(name: string): boolean
    emit(name: string, event: Event): void
}

type ElementKey = number

declare interface AttributeMap {
    [key: string]: string
}

declare class ElementEvent extends Event {
    dataSet: any
    cancelBubble: boolean
    readonly element: Element
    constructor(element: Element)
}

declare class Element extends EventEmitter {
    readonly elementId: ElementKey
    readonly name: string
    readonly document: Document
    readonly firstChild: Element | undefined
    readonly lastChild: Element | undefined
    readonly nextSibling: Element | undefined
    readonly prevSibling: Element | undefined
    readonly parent: Element | undefined
    append(element: Element): void
    appendTo(element: Element): void
    before(element: Element): void
    beforeTo(element: Element): void
    after(element: Element): void
    afterTo(element: Element): void
    remove(): void
    set(key: string, value: string | undefined): void
    get(key: string): string | undefined
    attributes(): AttributeMap
    setObject(key: string, object: any): void
    object(key: string): any
    dispatchEvent(name: string, event: Event): void
    toString(): string
    recycle(): void
    readonly dataSet: any
}

declare class StyleElement extends Element {
    status: string | undefined
    addStatus(s: string): void
    removeStatus(s: string): void
    hasStatus(s: string): boolean
    changedStatus(): void
}

declare class LayoutElement extends StyleElement {
    setFrame(x: number, y: number, width: number, height: number): void
    setContentSize(width: number, height: number): void
}

type LayoutFunction = (context: LayoutContext, element: LayoutElement) => void

declare class LayoutContext {
    setSize(width: number, height: number): void
    setUnit(name: string, scale: number, rate: number): void
    setLayout(name: string, func: LayoutFunction): void
    layout(element: LayoutElement): void
}

declare class Document extends EventEmitter {
    rootElement: Element | undefined
    createElement(name: string): Element
    elementsByName(name: string): Element[]
    set(name: string, library: string): void
    startTransaction(): void
    commitTransaction(): ArrayBuffer
    setTransaction(data: ArrayBuffer): void
    toString(): string
}


declare interface Library {
    [key: string]: any
}

declare interface Worker {
    terminate(): void
    postMessage(data: any): void
}

declare interface UICanvasContext {

}

declare interface UICanvas extends EventEmitter {
    getContext(name: string): UICanvasContext
    width: number
    height: number
}

declare enum ImageState {
    None = 0,
    Loading = 1,
    Error = 2,
    Loaded = 3
}

declare interface Image extends EventEmitter {
    readonly src: string
    readonly width: number
    readonly height: number
    readonly state: ImageState
}

declare class UIContext extends EventEmitter {
    openInputStream(uri: string): InputStream | undefined
    openOutputStream(uri: string): OutputStream | undefined
    exec(path: string, library?: Library): void
    getTextContent(path: string): string
    createWorker(path: string): Worker
    createCanvas(): UICanvas
    createImage(src: string): Image
}

declare class UIAttributedText {
    clear(): void
    appendText(text: string, font: string, color: string): void
    appendImage(image: Image, width?: number, height?: number, top?: number, left?: number, bottom?: number, right?: number): void
}

declare interface Size {
    width: number;
    height: number
}

declare class UIViewConfiguration {

}

declare enum UIWebViewUserScriptInjectionTime {
    Start = 0,
    End = 1
}

declare enum UIWebViewActionPolicy {
    Cancel = 0,
    Allow = 1
}

declare class UIWebViewConfiguration extends UIViewConfiguration {
    addUserScript(code: string, injectionTime: UIWebViewUserScriptInjectionTime): void
    addUserAction(pattern: string, policy: UIWebViewActionPolicy): void
}

declare enum UISubviewPosition {
    Front = 0,
    Back = 1
}

declare interface Point {
    x: number,
    y: number
}

declare interface UIView extends EventEmitter {
    set(key: string, value?: string): void
    setFrame(x: number, y: number, width: number, height: number): void
    setContentSize(width: number, height: number): void
    setContentOffset(x: number, y: number): void
    addSubview(v: UIView, position?: UISubviewPosition): void
    removeView(): void
    evaluateJavaScript(code: string): void
    setContent(content: string, contentType?: string, basePath?: string): void
    setAttributedText(text: UIAttributedText | undefined): void
    setImage(image: Image | undefined): void
    contentOffset(): Point
    createCanvas(worker?: Worker): UICanvas
    showToScreen(): void
}

type StorageLoadCallback = (value: string | undefined) => void
type StorageLoadKeysCallback = (keys: string[]) => void

declare interface Storage {
    get(key: string): string | undefined
    set(key: string, value: string): void
    load(key: string, cb: StorageLoadCallback): void
    clear(): void
    keys(): string[]
    loadKeys(cb: StorageLoadKeysCallback): void
}

declare class UIApp extends UIContext {
    static readonly kDataDirectory: string
    static readonly kTemporaryDirectory: string
    open(uri: string, animated: boolean): void
    back(delta: number, animated: boolean): void
    getAttributedTextContentSize(text: UIAttributedText, maxWidth: number): Size
    openDataBase(name: string): Database
    openFile(directory: string, path: string, type?: string): File
    openTempFile(prefix?: string, suffix?: string, type?: string): File
    createView(name: string, configuration?: UIViewConfiguration): UIView
    readonly appkey: string
    readonly storage: Storage
}

declare class UIViewElement extends LayoutElement {
    readonly view: UIView | undefined
    obtainView(context: UIViewContext): void
    recycleView(): void
}

declare class UIViewContext extends LayoutContext {
    constructor(app: UIApp)
    view: UIView | undefined
    page: UIPage | undefined
    obtainView(element: UIViewElement): void
}

declare interface UISpanElement extends StyleElement {

}

declare interface UIImgElement extends StyleElement {

}

declare interface UITextElement extends UIViewElement {
    readonly text: UIAttributedText | undefined
}


declare interface UIPageOptions {
    [key: string]: any
}

declare interface UIPage extends EventEmitter {
    readonly app: UIApp
    readonly view: UIView
    readonly width: number
    readonly height: number
    setOptions(options: UIPageOptions): void
    close(animated: boolean): void
}

declare interface UIScreen extends EventEmitter {
    readonly width: number
    readonly height: number
    readonly density: number
    readonly scale: number
}

declare interface AudioCodec {

}

declare interface Stream {
    close(): void
}

declare interface InputStream extends Stream {

}

declare class BufferInputStream implements InputStream {
    constructor(input: InputStream, size?: number)
    close(): void
}

declare interface OutputStream extends Stream {

}

declare class BufferOutputStream implements OutputStream {
    constructor(output: OutputStream, size?: number)
    flush(): boolean
    close(): void
}

declare class Audio {
    static readonly Ambient: number
    static readonly SoloAmbient: number
    static readonly Playback: number
    static readonly Record: number
    static readonly PlayAndRecord: number
    static startSession(category: number, cb: (errmsg: string | undefined) => void): void
}

declare class AudioQueue extends EventEmitter {
    readonly codec: AudioCodec
    start(): void
    stop(): void
    resume(): void
    pause(): void
    readonly duration: number
}

declare class AudioQueueInput extends AudioQueue {
    constructor(codec: AudioCodec, output: OutputStream);
    readonly output: OutputStream
}

declare class AudioQueueOutput extends AudioQueue {
    constructor(codec: AudioCodec, input: InputStream);
    readonly input: InputStream
}

declare class SpeexCodec implements AudioCodec {
    constructor()
    quality: number
}

declare class SpeexFileOutputStream implements OutputStream {
    constructor(output: OutputStream, codec?: SpeexCodec)
    readonly codec: SpeexCodec
    close(): void
}

declare class SpeexFileInputStream implements InputStream {
    constructor(input: InputStream)
    readonly codec: SpeexCodec | undefined
    close(): void
}

declare interface HeaderSet {
    [key: string]: string
}

declare class HTTPRequest extends EventEmitter {
    static ResponseTypeNone: number
    static ResponseTypeString: number
    static ResponseTypeArrayBuffer: number
    static ResponseTypeFile: number
    static ResponseTypeUnzip: number
    constructor()
    open(method: string, url: string, responseType: number): void
    send(value?: any): void
    cancel(): void
    setRequestHeader(key: string, value: string): void
    getResponseHeader(key: string): string | undefined
    readonly statusCode: number
    readonly responseText: string | undefined
    readonly responseFile: string | undefined
    readonly responseArrayBuffer: ArrayBuffer | undefined
    readonly responseHeaders: HeaderSet
}

declare type DatabaseValue = string | number | boolean | ArrayBuffer | undefined

declare interface DatabaseRow {
    [key: string]: DatabaseValue
}

declare interface Database {
    exec(sql: string, data: DatabaseValue[], cb: (id: number, errmsg: string | undefined) => void): void;
    query(sql: string, data: DatabaseValue[], cb: (items: DatabaseRow[], errmsg: string | undefined) => void): void;
    close(): void
}

declare interface Blob {
    readonly size: number
    readonly type: string
}

declare class File implements Blob {
    static open(uri: string, type?: string): File | undefined
    readonly size: number
    readonly type: string
    readonly name: string
    readonly lastModified: number
    remove(done?: () => void): void
    move(to: File, done?: () => void): void
    copy(to: File, done?: () => void): void
    openInputStream(): InputStream
    openOutputStream(append?: boolean): OutputStream
}

type FileList = File[]

declare class FileReader extends EventEmitter {
    static readonly EMPTY: number
    static readonly LOADING: number
    static readonly DONE: number
    readonly readyState: number
    readonly result: string | ArrayBuffer | undefined
    readonly error: string | undefined
    readonly md5: string
    readonly size: number
    readAsText(v: Blob | File): void
    readAsDataURL(v: Blob | File): void
    readAsArrayBuffer(v: Blob | File): void
    abort(): void;
    onload?: EventFunction
    onerror?: EventFunction
}

declare class URIQueryObject {
    [key: string]: string
}

declare class URI {
    constructor(v: string)
    scheme: string
    host: string
    hostname: string
    port: string
    query: string
    fragment: string
    path: string
    readonly queryObject: URIQueryObject
    get(key: string): string
    set(keu: string, value: string): void
    toString(): string
}

declare var app: UIApp
declare var userAgent: string
declare var platform: string
declare var path: string
declare var query: any
declare var page: UIPage
declare var screen: UIScreen
declare function compile(code: string, path: string): any


declare class Promise<T> {

    constructor(executor: (resolve: (value: T) => void, reject: (reason: any) => void) => void)

    then(onfulfilled?: (value: T) => T | Promise<T> | undefined, onrejected?: (reason: any) => void): void

    catch(onrejected?: (reason: any) => void): void

    finally(onfinally?: () => void): void

    static all(iterable: Promise<any>[]): Promise<any>
    static race(iterable: Promise<any>[]): Promise<any>
    static resolve(value: any): Promise<any>
    static reject(reason: any): Promise<any>
}
