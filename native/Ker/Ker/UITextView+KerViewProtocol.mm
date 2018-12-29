//
//  UITextView+KerViewProtocol.m
//  KK
//
//  Created by zhanghailong on 2018/11/8.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import "UITextView+KerViewProtocol.h"
#import "UIFont+Ker.h"
#import "UIColor+Ker.h"

#import "KerObject.h"
#include <ui/ui.h>
#include <ui/view.h>
#include <objc/runtime.h>

#import "KerApp.h"

@interface UITextViewKKViewProtocol : NSObject<UITextViewDelegate>

@property(nonatomic,assign) kk::Uint64 viewId;
@property(nonatomic,weak) KerApp * app;

@end

@implementation UITextViewKKViewProtocol

- (BOOL)textView:(UITextView *)textView shouldChangeTextInRange:(NSRange)range replacementText:(NSString *)string {
    
    if(_viewId != 0) {
        NSString * text = [textView.text stringByReplacingCharactersInRange:range withString:string];
        [_app emit:@"change" viewId:_viewId data:@{@"value":text}];
    }
    
    return YES;
}

- (BOOL)textViewShouldBeginEditing:(UITextView *)textView {
    
    if(_viewId != 0) {
        [_app emit:@"focus" viewId:_viewId data:nil];
    }
    
    return YES;
}

- (BOOL)textViewShouldEndEditing:(UITextView *)textView {
    
    if(_viewId != 0) {
        [_app emit:@"blur" viewId:_viewId data:nil];
    }
    
    return YES;
}


@end

@implementation UITextView (KerViewProtocol)

-(void) KerViewObtain:(KerViewId) viewId app:(KerApp *)app {
    [super KerViewObtain:viewId app:app];
    UITextViewKKViewProtocol * object = [[UITextViewKKViewProtocol alloc] init];
    object.viewId = viewId;
    object.app = app;
    self.delegate = object;
    objc_setAssociatedObject(self, "__UITextViewKKViewProtocol", object, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}

-(void) KerViewRecycle:(KerViewId) viewId app:(KerApp *)app {
    [super KerViewRecycle:viewId app:app];
    UITextViewKKViewProtocol * object = objc_getAssociatedObject(self, "__UITextViewKKViewProtocol");
    if(object) {
        self.delegate = nil;
        objc_setAssociatedObject(self, "__UITextViewKKViewProtocol", nil, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    }
}

-(void) KerView:(KerViewId) viewId setAttribute:(NSString *) key value:(NSString *) value app:(KerApp *)app {
    
    [super KerView:viewId setAttribute:key value:value app:app];
    
    if(key == nil) {
        return ;
    }
    
    if([key isEqualToString:@"value"]) {
        self.text = value;
    } else if([key isEqualToString:@"type"]) {
        if([value isEqualToString:@"text"]) {
            self.keyboardType = UIKeyboardTypeDefault;
        } else if([value isEqualToString:@"number"]) {
            self.keyboardType = UIKeyboardTypeNumberPad;
        } else if([value isEqualToString:@"digit"]) {
            self.keyboardType = UIKeyboardTypeNumbersAndPunctuation;
        } else {
            self.keyboardType = UIKeyboardTypeDefault;
        }
    } else if([key isEqualToString:@"disabled"]) {
        self.editable = !(value != nil && ![value isEqualToString:@"false"]);
    } else if([key isEqualToString:@"focus"]) {
        if(value != nil && [value  isEqualToString:@"true"]) {
            [self becomeFirstResponder];
        }
    } else if([key isEqualToString:@"confirm-type"]) {
        if([value isEqualToString:@"done"]) {
            self.returnKeyType = UIReturnKeyDone;
        } else if([value isEqualToString:@"send"]) {
            self.returnKeyType = UIReturnKeySend;
        } else if([value isEqualToString:@"search"]) {
            self.returnKeyType = UIReturnKeySearch;
        } else if([value isEqualToString:@"next"]) {
            self.returnKeyType = UIReturnKeyNext;
        } else if([value isEqualToString:@"go"]) {
            self.returnKeyType = UIReturnKeyGo;
        } else {
            self.returnKeyType = UIReturnKeyDone;
        }
    } else if([key isEqualToString:@"color"]) {
        self.textColor = [UIColor colorWithKerCString:[value UTF8String]];
    } else if([key isEqualToString:@"font"]) {
        self.font = [UIFont fontWithKerCString:[value UTF8String]];
        
    } else {
        
        kk::ui::TextAlign v = kk::ui::TextAlignFromString([value UTF8String]);
        
        switch (v) {
            case kk::ui::TextAlignEnd:
            case kk::ui::TextAlignRight:
                self.textAlignment = NSTextAlignmentRight;
                break;
            case kk::ui::TextAlignCenter:
                self.textAlignment = NSTextAlignmentCenter;
                break;
            default:
                self.textAlignment = NSTextAlignmentLeft;
                break;
        }
        
    }
    
}

@end



