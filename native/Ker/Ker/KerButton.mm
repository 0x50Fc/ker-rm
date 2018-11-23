//
//  KerButton.m
//  Ker
//
//  Created by hailong11 on 2018/11/21.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import "KerButton.h"

#include <ui/ui.h>
#include <ui/view.h>
#include <objc/runtime.h>

@interface KerButton() {
    kk::ui::View * _view;
}

@end


@implementation KerButton

static kk::CString KerViewUITouchPhaseCString(UITouchPhase phase) {
    switch (phase) {
        case UITouchPhaseBegan:
        case UITouchPhaseEnded:
            return "end";
        case UITouchPhaseMoved:
            return "move";
        case UITouchPhaseCancelled:
            return "cancel";
        default:
            return "move";
    }
}

- (void) event:(NSString *) name touches:(NSSet<UITouch *> *)touches withEvent:(nullable UIEvent *)event {
    
    kk::Strong<kk::Event> e = new kk::Event();
    
    kk::TObject<kk::String, kk::Any> * data = new kk::TObject<kk::String, kk::Any>();
    
    (*data)["timestamp"] = (kk::Double) [event timestamp];
    
    kk::Array<kk::Any> * items = new kk::Array<kk::Any>();
    
    for(UITouch * touch in touches) {
        kk::TObject<kk::String, kk::Any> * item = new kk::TObject<kk::String, kk::Any>();
        CGPoint p = [touch locationInView:self];
        (*item)["x"] = (kk::Double) p.x;
        (*item)["y"] = (kk::Double) p.y;
        (*item)["type"] = KerViewUITouchPhaseCString(touch.phase);
        (*item)["id"] = (kk::CString) [[NSString stringWithFormat:@"0x%lx",(long)(__bridge void *) touch] UTF8String];
        kk::Any v(item);
        items->push(v);
    }
    
    (*data)["touches"] = items;
    
    e->setData(data);
    
    _view->emit([name UTF8String], e);
    
}

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(nullable UIEvent *)event {
    [super touchesBegan:touches withEvent:event];
    [self event:@"touchstart" touches:touches withEvent:event];
}

- (void)touchesMoved:(NSSet<UITouch *> *)touches withEvent:(nullable UIEvent *)event {
    [super touchesMoved:touches withEvent:event];
    [self event:@"touchmove" touches:touches withEvent:event];
}

- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(nullable UIEvent *)event {
    [super touchesEnded:touches withEvent:event];
    [self event:@"touchend" touches:touches withEvent:event];
}

- (void)touchesCancelled:(NSSet<UITouch *> *)touches withEvent:(nullable UIEvent *)event {
    [super touchesCancelled:touches withEvent:event];
    [self event:@"touchcancel" touches:touches withEvent:event];
}

-(void) KerViewObtain:(void *) view {
    _view = (kk::ui::View *) view;
}

-(void) KerViewRecycle:(void *) view {
    _view = nullptr;
}

- (void)endTrackingWithTouch:(nullable UITouch *)touch withEvent:(nullable UIEvent *)event {
    
    if(CGRectContainsPoint(self.bounds, [touch locationInView:self])) {
        [self event:@"tap" touches:[NSSet setWithObject:touch] withEvent:event];
    }
    
    [super endTrackingWithTouch:touch withEvent:event];
}

@end
