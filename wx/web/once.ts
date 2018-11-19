
var _running:boolean = false;
var _funcs:(()=>void)[] = [];

function run():void {
    var fn:(()=>void) | undefined;
    while((fn = _funcs.shift())) {
        fn();
    }
    _running = true;
}

export function once(func:()=>void):void {

    _funcs.push(func);

    if(!_running) {
        _running = true;
        setTimeout(function(){
            run();
        },0);
    }

}

