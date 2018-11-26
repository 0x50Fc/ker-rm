
export function booleanValue(v:string|undefined):boolean {
    return v !== undefined && v != 'false' && v != '0';
}

export function pixelValue(v:string|undefined):number {
    
    if(v && v.endsWith("rpx")) {
        var r = parseFloat(document.documentElement!.style.fontSize!) || 14;
        return parseFloat(v) * r * 0.05;
    } else if(v) {
        return parseFloat(v) || 0;
    }

    return 0;
}

export function pixelStringValue(v:string|undefined):string {
    if(v && v.endsWith("rpx")) {
        return (parseFloat(v) * 0.05) + 'rem';
    } 
    return v as string;
}
