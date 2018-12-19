//
//  KerWXObject+DeviceVibrate.h
//  KerWX
//
//  Created by zuowu on 2018/12/19.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import <KerWX/KerWX.h>

@interface KerWXObject (DeviceVibrate)

-(void) vibrateLong:(KerJSObject *) object;
-(void) vibrateShort:(KerJSObject *) object;

@end


