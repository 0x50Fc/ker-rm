//
//  KerPage.m
//  KK
//
//  Created by zhanghailong on 2018/10/31.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#import "KerPage.h"
#import "KerObject.h"
#include <ui/ui.h>
#include <ui/page.h>

namespace kk {
    namespace ui {
        extern kk::Strong<View> createView(CFTypeRef view,kk::ui::ViewConfiguration * config,kk::ui::Context * context);
    }
}

@interface KerPage() {
    kk::ui::Page * _page;
}

-(void) setOptions:(id) options;

@end

@implementation KerPage

-(instancetype) initWithView:(UIView *) view app:(KerApp *) app {
    if((self = [super init])) {
        _app = app;
        _view = view;
        kk::Strong<kk::ui::View> v = kk::ui::createView((__bridge CFTypeRef) view,nullptr,(kk::ui::Context *) [app CPointer]);
        _page = new kk::ui::Page((kk::ui::App *)[_app CPointer],v);
        {
            kk::ui::Size s = {(kk::ui::Float)view.bounds.size.width,(kk::ui::Float)view.bounds.size.height};
            _page->setSize(s);
        }
        _page->retain();
        
        CFTypeRef page = (__bridge CFTypeRef) self;
        
        _page->on("options", new kk::TFunction<void,kk::Event *>([page](kk::Event * event)->void{
            
            @autoreleasepool {
                KerPage * p = (__bridge KerPage *) page;
                kk::Any v = event->data();
                id object = KerObjectFromAny(v);
                [p setOptions:object];
            }
            
        }));
        
    }
    return self;
}

-(void) dealloc{
    if(_page) {
        _page->off();
        _page->release();
    }
}

-(void) recycle {
    if(_page) {
        _page->off();
        _page->release();
        _page = nullptr;
    }
}

-(KerPageCPointer) CPointer {
    return _page;
}

-(void) run:(NSString *) path query:(NSDictionary<NSString *,NSString *> *) query {
    if(_page == nullptr) {
        return;
    }
    kk::Strong<kk::TObject<kk::String, kk::String>> q = new kk::TObject<kk::String, kk::String>();
    NSEnumerator * keyEnum = [query keyEnumerator];
    NSString * key;
    while((key = [keyEnum nextObject])) {
        NSString * value = [query valueForKey:key];
        (*q)[[key UTF8String]] = [value UTF8String];
    }
    _page->run([path UTF8String], q);
}

-(void) setSize:(CGSize) size {
    if(_page == nullptr) {
        return;
    }
    kk::ui::Size s = {(kk::ui::Float)size.width,(kk::ui::Float)size.height};
    _page->setSize(s);
}

-(void) setOptions:(id) options {
    if([(id)_delegate respondsToSelector:@selector(KerPage:setOptions:)]) {
        [_delegate KerPage:self setOptions:options];
    }
}

@end
