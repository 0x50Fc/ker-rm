//
//  KerCanvasView.m
//  Ker
//
//  Created by hailong11 on 2019/2/12.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#import "KerCanvasView.h"

@implementation KerGLContext

-(instancetype) init {
    if((self = [super init])) {
        
        _GLContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
        [_GLContext setMultiThreaded:YES];
        
        [EAGLContext setCurrentContext:_GLContext];
        glGenFramebuffers(1, &_framebuffer);
        glGenRenderbuffers(1, &_renderbuffer);
        glGenRenderbuffers(1, &_depthbuffer);
        
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE , GL_ONE_MINUS_SRC_ALPHA);
        
    }
    return self;
}

-(void) resizeGLContext:(GLsizei) width height:(GLsizei) height {
    
    [EAGLContext setCurrentContext:_GLContext];
    
    if(_width != width || _height != height) {
        _width = width;
        _height = height;
        
        glBindRenderbuffer(GL_RENDERBUFFER, _renderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA
                              , _width, _height);
        
        glBindRenderbuffer(GL_RENDERBUFFER, _depthbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16
                              , _width, _height);
        
        glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _renderbuffer);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthbuffer);
        
        glViewport(0,0,_width,_height);
        
        glBindRenderbuffer(GL_RENDERBUFFER, _renderbuffer);
        
    } else {
        glBindRenderbuffer(GL_RENDERBUFFER, _renderbuffer);
    }
    
}

-(void) resizeGLContext:(CAEAGLLayer *) layer {
    
    [EAGLContext setCurrentContext:_GLContext];
    
    glBindRenderbuffer(GL_RENDERBUFFER, _renderbuffer);
    
    [_GLContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:layer];
    
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &_width);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &_height);
    
    glBindRenderbuffer(GL_RENDERBUFFER, _depthbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16
                          , _width, _height);
    
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _renderbuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthbuffer);
    
    glViewport(0,0,_width,_height);
    
    glBindRenderbuffer(GL_RENDERBUFFER, _renderbuffer);
    
    [EAGLContext setCurrentContext:nil];
    
}

-(void) displayGLContext {
    [EAGLContext setCurrentContext:_GLContext];
    glBindRenderbuffer(GL_RENDERBUFFER, _renderbuffer);
    [_GLContext presentRenderbuffer:GL_RENDERBUFFER];
}


@end

@interface KerCanvasView() {

}

@end

@implementation KerCanvasView

+(Class) layerClass {
    return [CAEAGLLayer class];
}

-(CAEAGLLayer *) GLLayer {
    return (CAEAGLLayer *) self.layer;
}

@end
