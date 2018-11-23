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

@interface UITextFieldKKViewProtocol : NSObject<UITextFieldDelegate>

@property(nonatomic,assign) kk::ui::View * view;

@end

@implementation UITextFieldKKViewProtocol

- (BOOL)textField:(UITextField *)textField shouldChangeCharactersInRange:(NSRange)range replacementString:(NSString *)string {
    
    if(_view) {
        NSString * text = [textField.text stringByReplacingCharactersInRange:range withString:string];
        kk::Strong<kk::Event> e = new kk::Event();
        kk::Strong<kk::TObject<kk::String, kk::String>> data = new kk::TObject<kk::String, kk::String>({{"value",[text UTF8String]}});
        e->setData(data);
        _view->emit("change", e);
    }
    
    return YES;
}

- (BOOL)textFieldShouldBeginEditing:(UITextField *)textField {
    
    if(_view) {
        kk::Strong<kk::Event> e = new kk::Event();
        _view->emit("focus", e);
    }
    
    return YES;
}

- (BOOL)textFieldShouldEndEditing:(UITextField *)textField {
    
    if(_view) {
        kk::Strong<kk::Event> e = new kk::Event();
        _view->emit("blur", e);
    }
    
    return YES;
}

- (BOOL)textFieldShouldReturn:(UITextField *)textField {
    
    if(_view) {
        kk::Strong<kk::Event> e = new kk::Event();
        _view->emit("done", e);
    }
    
    return YES;
}

@end

@implementation UITextField (KerViewProtocol)

-(void) KerViewObtain:(KerViewCPointer) view {
    [super KerViewObtain:view];
    UITextFieldKKViewProtocol * object = [[UITextFieldKKViewProtocol alloc] init];
    object.view = (kk::ui::View *) view;
    self.delegate = object;
    objc_setAssociatedObject(self, "__UITextFieldKKViewProtocol", object, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}

-(void) KerViewRecycle:(KerViewCPointer) view {
    [super KerViewRecycle:view];
    UITextFieldKKViewProtocol * object = objc_getAssociatedObject(self, "__UITextFieldKKViewProtocol");
    if(object) {
        self.delegate = nil;
        objc_setAssociatedObject(self, "__UITextFieldKKViewProtocol", nil, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    }
}

-(void) KerView:(KerViewCPointer) view setAttribute:(const char *) key value:(const char *) value {
    
    [super KerView:view setAttribute:key value:value];
    
    if(key == nullptr) {
        return ;
    }
    
    if(strcmp(key, "value") == 0) {
        self.text = value? [NSString stringWithCString:value encoding:NSUTF8StringEncoding] : nil;
    } else if(strcmp(key, "type") == 0) {
        if(value == nullptr || strcmp(value, "text") == 0) {
            self.keyboardType = UIKeyboardTypeDefault;
        } else if(strcmp(value, "number") == 0) {
            self.keyboardType = UIKeyboardTypeNumberPad;
        } else if(strcmp(value, "digit") == 0) {
            self.keyboardType = UIKeyboardTypeNumbersAndPunctuation;
        } else {
            self.keyboardType = UIKeyboardTypeDefault;
        }
    } else if(strcmp(key, "password") == 0) {
        self.secureTextEntry = value != nullptr && strcmp(value, "true") == 0;
    } else if(strcmp(key, "placeholder") == 0) {
        self.placeholder = value? [NSString stringWithCString:value encoding:NSUTF8StringEncoding] : nil;
    } else if(strcmp(key, "disabled") == 0) {
        self.enabled = !(value != nullptr && strcmp(value, "true") == 0);
    } else if(strcmp(key, "focus") == 0) {
        if(value != nullptr && strcmp(value, "true") == 0) {
            [self becomeFirstResponder];
        }
    } else if(strcmp(key, "confirm-type") == 0) {
        if(value == nullptr || strcmp(value, "done") == 0) {
            self.returnKeyType = UIReturnKeyDone;
        } else if(strcmp(value, "send") == 0) {
            self.returnKeyType = UIReturnKeySend;
        } else if(strcmp(value, "search") == 0) {
            self.returnKeyType = UIReturnKeySearch;
        } else if(strcmp(value, "next") == 0) {
            self.returnKeyType = UIReturnKeyNext;
        } else if(strcmp(value, "go") == 0) {
            self.returnKeyType = UIReturnKeyGo;
        } else {
            self.returnKeyType = UIReturnKeyDone;
        }
    } else if(strcmp(key, "color") == 0) {
        self.textColor = [UIColor colorWithKerCString:value];
    } else if(strcmp(key, "font") == 0) {
        self.font = [UIFont fontWithKerCString:value];
    } else if(strcmp(key, "text-align") == 0){
        kk::ui::TextAlign v = kk::ui::TextAlignFromString(value);
        
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
        
    } else if(strcmp(key, "padding") == 0){
        kk::ui::Edge padding(value);
        
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


