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
#import "KerUI.h"

@interface KerPage() {
    id _options;
}

@end

@implementation KerPage

-(instancetype) initWithPage:(KerPageCPointer) page {
    if((self = [super init])) {
        _page = page;
        _page->retain();
    }
    return self;
}

-(void) dealloc {
    if(_page) {
        kk::ui::Page * page = _page;
        _page = nullptr;
        page->queue()->async([page]()->void{
            page->release();
        });
    }
}

-(void) recycle {
    if(_page) {
        
        kk::ui::Page * page = _page;
        _page = nullptr;
        [KerUI removePage:page->pageId()];
        page->queue()->async([page]()->void{
            page->release();
        });
    }
}

-(void) setOptions:(id) options  {
    _options = options;
    if([(id)_delegate respondsToSelector:@selector(KerPage:setOptions:)]) {
        [_delegate KerPage:self setOptions:options];
    }
}

-(void) close:(BOOL) animated {
    if([(id)_delegate respondsToSelector:@selector(KerPage:close:)]) {
        [_delegate KerPage:self close:animated];
    }
}

-(void) setSize:(CGSize) size {
    if(_page) {
        kk::Weak<kk::ui::Page> p = _page;
        kk::ui::Size s = { (kk::ui::Float) size.width,  (kk::ui::Float) size.height };
        _page->queue()->async([s,p]()->void{
            kk::Strong<kk::ui::Page> page = p.operator->();
            if(page !=nullptr) {
                if(page->view() != nullptr) {
                    page->setSize((kk::ui::Size &) s);
                }
            }
        });
    }
}

-(void) open:(UIView *) view {
    
    _view = view;
    
    if(_page) {
        kk::Strong<kk::NativeObject> vv = new kk::NativeObject((__bridge kk::Native *) view);
        kk::Weak<kk::ui::Page> p = _page;
        CGSize size = view.bounds.size;
        kk::ui::Rect r = { {0,0} ,{(kk::ui::Float) size.width,  (kk::ui::Float) size.height} };
        if(_options != nil) {
            if([(id)_delegate respondsToSelector:@selector(KerPage:setOptions:)]) {
                [_delegate KerPage:self setOptions:_options];
            }
        }
        _page->queue()->async([r,p,vv]()->void{
            @autoreleasepool {
                kk::Strong<kk::ui::Page> page = p.operator->();
                if(page !=nullptr) {
                    kk::Strong<kk::ui::View> v = page->app()->createView(vv.operator->()->native(), (kk::ui::Rect &) r);
                    page->setView(v);
                    page->setSize((kk::ui::Size &) r.size);
                    page->ready();
                }
            }
        });
    }
    
}

-(void) viewDidLayoutSubviews {
    if(_page && _view) {
        kk::ui::Size s = {(kk::ui::Float) _view.bounds.size.width,(kk::ui::Float)_view.bounds.size.height};
        kk::Weak<kk::ui::Page> p = _page;
        _page->queue()->async([s,p]()->void{
            @autoreleasepool {
                kk::Strong<kk::ui::Page> page = p.operator->();
                if(page !=nullptr) {
                    page->setSize((kk::ui::Size&) s);
                }
            }
        });
    }
}



@end
