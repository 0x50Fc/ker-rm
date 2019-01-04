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

#import "KerUI.h"

@interface KerButton() {
    
}

@property(nonatomic,assign) kk::Uint64 viewId;
@property(nonatomic,assign) KerId app;

@end


@implementation KerButton

static NSString * KerViewUITouchPhaseCString(UITouchPhase phase) {
    switch (phase) {
        case UITouchPhaseBegan:
        case UITouchPhaseEnded:
            return @"end";
        case UITouchPhaseMoved:
            return @"move";
        case UITouchPhaseCancelled:
            return @"cancel";
        default:
            return @"move";
    }
}

- (void) event:(NSString *) name touches:(NSSet<UITouch *> *)touches withEvent:(nullable UIEvent *)event {
    
    if(_viewId == 0 || _app == 0) {
        return;
    }
    NSMutableDictionary * data = [NSMutableDictionary dictionaryWithCapacity:4];
    
    data[@"timestamp"] = @([event timestamp]);
    
    NSMutableArray * items = [NSMutableArray arrayWithCapacity:4];
    
    data[@"touches"] = items;
    
    for(UITouch * touch in touches) {
        NSMutableDictionary * item = [NSMutableDictionary dictionaryWithCapacity:4];
        CGPoint p = [touch locationInView:self];
        
        item[@"x"] = @(p.x);
        item[@"y"] = @(p.y);
        item[@"type"] = KerViewUITouchPhaseCString(touch.phase);
        item[@"id"] = [NSString stringWithFormat:@"0x%lx",(long)(__bridge void *) touch];
        
        [items addObject:item];
    }
    
    [KerUI app:_app emit:name viewId:_viewId data:data];
    
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

-(void) KerViewObtain:(KerId) viewId app:(KerId) app {
    [super KerViewObtain:viewId app:app];
    _viewId = viewId;
    _app = app;
}

-(void) KerViewRecycle:(KerId) viewId app:(KerId) app {
    [super KerViewRecycle:viewId app:app];
    _viewId = 0;
    _app = 0;
}

- (void)endTrackingWithTouch:(nullable UITouch *)touch withEvent:(nullable UIEvent *)event {
    
    if(CGRectContainsPoint(self.bounds, [touch locationInView:self])) {
        [self event:@"tap" touches:[NSSet setWithObject:touch] withEvent:event];
    }
    
    [super endTrackingWithTouch:touch withEvent:event];
}

@end
