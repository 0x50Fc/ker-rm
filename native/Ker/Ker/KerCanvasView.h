//
//  KerCanvasView.h
//  Ker
//
//  Created by hailong11 on 2019/2/12.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#import <Ker/KerView.h>
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#include <OpenGLES/EAGL.h>

@interface KerGLContext : NSObject

@property(nonatomic,assign,readonly) GLsizei width;
@property(nonatomic,assign,readonly) GLsizei height;
@property(nonatomic,assign,readonly) GLuint framebuffer;
@property(nonatomic,assign,readonly) GLuint renderbuffer;
@property(nonatomic,assign,readonly) GLuint depthbuffer;
@property(nonatomic,strong,readonly) EAGLContext * GLContext;

-(void) resizeGLContext:(GLsizei) width height:(GLsizei) height;

-(void) resizeGLContext:(CAEAGLLayer *) layer;

-(void) displayGLContext;

@end

@interface KerCanvasView : KerView

@property(nonatomic,strong,readonly) CAEAGLLayer * GLLayer;

@end
