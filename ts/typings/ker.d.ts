

declare class Event {
    data: any
    returnValue: any
    v:String
}

type EventFunction = (event: Event,name:string) => void

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

declare interface UIContext extends EventEmitter {
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

declare interface SqliteResultSet {
    getColumnCount(): number
    getColumnName(index: number): string
    getValue(index: number): any
    next(): boolean
    close(): void
}

declare interface Sqlite {
    exec(sql: String, args: any[]): void
    query(sql: String, args: any[]): SqliteResultSet
    lastInsertRowId(): number
    close(): void
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

declare interface UIApp extends UIContext {
    open(uri: string, animated: boolean): void
    back(delta: number, animated: boolean): void
    getAttributedTextContentSize(text: UIAttributedText, maxWidth: number): Size
    createSqlite(path: string): Sqlite
    createView(name: string, configuration: UIViewConfiguration): UIView
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

declare var app: UIApp
declare var userAgent: string
declare var platform: string
declare var path:string
declare var query:any
declare var page: UIPage

