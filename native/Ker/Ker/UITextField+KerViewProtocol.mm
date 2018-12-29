//
//  UITextField+KerViewProtocol.m
//  KK
//
//  Created by zhanghailong on 2018/11/8.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import "UITextField+KerViewProtocol.h"
#import "UIFont+Ker.h"
#import "UIColor+Ker.h"
#import "KerObject.h"
#include <ui/ui.h>
#include <ui/view.h>
#include <objc/runtime.h>
#import "KerApp.h"

@interface UITextFieldKKViewProtocol : NSObject<UITextFieldDelegate>

@property(nonatomic,assign) kk::Uint64 viewId;
@property(nonatomic,weak) KerApp * app;

@end

@implementation UITextFieldKKViewProtocol

- (BOOL)textField:(UITextField *)textField shouldChangeCharactersInRange:(NSRange)range replacementString:(NSString *)string {
    
    if(_viewId != 0) {
        NSString * text = [textField.text stringByReplacingCharactersInRange:range withString:string];
        [_app emit:@"chnage" viewId:_viewId data:@{@"value":text}];
    }
    
    return YES;
}

- (BOOL)textFieldShouldBeginEditing:(UITextField *)textField {
    
    if(_viewId) {
        [_app emit:@"focus" viewId:_viewId data:nil];
    }
    
    return YES;
}

- (BOOL)textFieldShouldEndEditing:(UITextField *)textField {
    
    if(_viewId) {
        [_app emit:@"blur" viewId:_viewId data:nil];
    }
    
    return YES;
}

- (BOOL)textFieldShouldReturn:(UITextField *)textField {
    
    if(_viewId) {
        [_app emit:@"done" viewId:_viewId data:nil];
    }
    
    return YES;
}

@end

@implementation UITextField (KerViewProtocol)

-(void) KerViewObtain:(KerViewId) viewId app:(KerApp *)app {
    [super KerViewObtain:viewId app:app];
    UITextFieldKKViewProtocol * object = [[UITextFieldKKViewProtocol alloc] init];
    object.viewId = viewId;
    object.app = app;
    self.delegate = object;
    objc_setAssociatedObject(self, "__UITextFieldKKViewProtocol", object, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}

-(void) KerViewRecycle:(KerViewId) viewId app:(KerApp *)app {
    [super KerViewRecycle:viewId app:app];
    UITextFieldKKViewProtocol * object = objc_getAssociatedObject(self, "__UITextFieldKKViewProtocol");
    if(object) {
        self.delegate = nil;
        objc_setAssociatedObject(self, "__UITextFieldKKViewProtocol", nil, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    }
}

-(void) KerView:(KerViewId) viewId setAttribute:(NSString *) key value:(NSString *) value app:(KerApp *) app {
    
    [super KerView:viewId setAttribute:key value:value app:app];
    
    if(key == nil) {
        return ;
    }
    
    if([key isEqualToString:@"value"]) {
        self.text =value;
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
    } else if([key isEqualToString:@"password"]) {
        self.secureTextEntry = value != nil && ![value isEqualToString:@"false"];
    } else if([key isEqualToString:@"placeholder"]) {
        self.placeholder = value;
    } else if([key isEqualToString:@"disabled"]) {
        self.enabled = value != nil && ![value isEqualToString:@"false"];
    } else if([key isEqualToString:@"focus"]) {
        if(value != nil && ![value isEqualToString:@"false"]) {
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
    } else if([key isEqualToString:@"text-align"]){
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
        
    } else if([key isEqualToString:@"padding"]){
        kk::ui::Edge padding([value UTF8String]);
        
        if(padding.left <= 0) {
            self.leftView = nil;
            self.leftViewMode = UITextFieldViewModeNever;
        } else {
            self.leftView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, padding.left, self.bounds.size.height)];
            self.leftView.userInteractionEnabled = NO;
            self.leftViewMode = UITextFieldViewModeAlways;
        }
        
        if(padding.right <= 0) {
            self.rightView = nil;
            self.rightViewMode = UITextFieldViewModeNever;
        } else {
            self.rightView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, padding.right, self.bounds.size.height)];
            self.rightView.userInteractionEnabled = NO;
            self.rightViewMode = UITextFieldViewModeAlways;
        }
        
    }
    
}

@end


