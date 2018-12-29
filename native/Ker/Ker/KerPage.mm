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
#import "KerWeak.h"

namespace kk {
    namespace ui {
        extern kk::Strong<View> createView(CFTypeRef view,kk::ui::ViewConfiguration * config,kk::ui::Context * context);
    }
}

@interface KerPage() {
    kk::ui::Page * _page;
}

-(void) setOptions:(id) options;

-(void) close:(BOOL) animated;

@end

@implementation KerPage

-(instancetype) initWithView:(UIView *) view app:(KerApp *) app{
    if((self = [super init])) {
        _app = app;
        _view = view;
        
        kk::ui::App * a = [app CPointer];
        
        kk::ui::Rect frame = {
            {(kk::ui::Float)view.frame.origin.x,(kk::ui::Float)view.frame.origin.y},
            {(kk::ui::Float)view.frame.size.width,(kk::ui::Float)view.frame.size.height}};
        
        KerWeak * native = [[KerWeak alloc] init];
        
        native.object = self;
        
        a->queue()->sync([self,native,frame,a,view]()->void{
            
            kk::Strong<kk::ui::View> v = a->createView((__bridge kk::Native *) view,(kk::ui::Rect &) frame);
            
            kk::Strong<kk::ui::Page> page = a->createPage(v);
            _page = page;
            _page->retain();
            _page->setNative((__bridge kk::Native *) native);
            page->setSize((kk::ui::Size &) frame.size);
            
        });
    
    }
    return self;
}

-(void) dealloc{
    if(_page) {
        kk::ui::Page * page = _page;
        _page = nullptr;
        page->queue()->async([page]()->void{
            page->off();
            page->release();
        });
    }
}

-(void) recycle {
    self.delegate = nil;
    if(_page) {
        kk::ui::Page * page = _page;
        _page = nullptr;
        page->queue()->async([page]()->void{
            page->off();
            page->release();
        });
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
    kk::String p = [path UTF8String];
    kk::ui::Page * page = _page;
    
    _page->queue()->async([p,q,page]()->void{
        page->run(p.c_str(), q.operator->());
    });
    
}

-(void) setSize:(CGSize) size {
    if(_page == nullptr) {
        return;
    }
    kk::ui::Size s = {(kk::ui::Float)size.width,(kk::ui::Float)size.height};
    kk::ui::Page * page = _page;
    _page->queue()->async([s,page]()->void{
        page->setSize((kk::ui::Size &) s);
    });
}

-(void) setOptions:(id) options {
    if([(id)_delegate respondsToSelector:@selector(KerPage:setOptions:)]) {
        [_delegate KerPage:self setOptions:options];
    }
}

-(void) close:(BOOL) animated {
    if([(id)_delegate respondsToSelector:@selector(KerPage:close:)]) {
        [_delegate KerPage:self close:animated];
    }
}

@end
