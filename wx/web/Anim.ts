

interface FrameMap {
    [key:string]:(cur:number)=>boolean
}

var autoId:number = 0;
var cur:number = 0;
var speed:number = 1000 / 60;
var frames:FrameMap = {};

function add(frame:(cur:number)=>boolean):number {
    var id = ++ autoId;
    frames[id] = frame;
    return id;
}

function remove(id:number):void {
    delete frames[id];
}

setInterval(():void=>{
    cur += speed;
    var rm:string[] = [];
    for(var id in frames) {
        var fn = frames[id];
        if(fn(cur) === false) {
            rm.push(id);
        }
    }
    for(var id of rm) {
        delete frames[id];
    }
},speed);

export function startAnimation(duration:number,func:(v:number)=>void):number {
    let start = cur;
    return add((cur:number):boolean=>{
        let v = (cur - start) / duration;
        if(v >= 1) {
            v = 1;
            func(v);
            return false;
        }
        func(v);
        return true;
    });
};

export function clearAnimation(id:number):void {
    remove(id);
};

