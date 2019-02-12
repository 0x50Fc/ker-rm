//
//  GLContext.h
//  Ker
//
//  Created by zhanghailong on 2019/2/12.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#ifndef gl_GLContext_h
#define gl_GLContext_h

#include <core/kk.h>

#if defined(__APPLE__)

#if TARGET_OS_OSX
#include <OpenGL/OpenGL.h>
#include <OpenGL/gl.h>
#else
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#endif

#elif defined(__ANDROID_API__)

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#endif


namespace kk {
    
    namespace GL {
        
        class GLContext;
        
        class GLObject : public kk::Object {
        public:
            GLObject(GLContext * context,GLuint value);
            virtual ~GLObject();
            virtual GLuint value();
            virtual kk::Boolean isValid();
            virtual void recycle();
            
            Ker_CLASS(GLObject, Object, "GLObject")
            
        protected:
            virtual void onRecycle();
            GLuint _value;
            GLContext * _context;
        };
        
        class GLBuffer : public GLObject {
        public:
            GLBuffer(GLContext * context,GLuint value);
            Ker_CLASS(GLBuffer, GLObject, "GLBuffer")
        protected:
            virtual void onRecycle();
        };
        
        class GLTexture : public GLObject {
        public:
            GLTexture(GLContext * context,GLuint value);
            Ker_CLASS(GLTexture, GLObject, "GLTexture")
        protected:
            virtual void onRecycle();
        };
        
        class GLProgram : public GLObject {
        public:
            GLProgram(GLContext * context,GLuint value);
            Ker_CLASS(GLProgram, GLObject, "GLProgram")
        protected:
            virtual void onRecycle();
        };
        
        class GLShader : public GLObject{
        public:
            GLShader(GLContext * context,GLuint value);
            Ker_CLASS(GLShader, GLObject, "GLShader")
        protected:
            virtual void onRecycle();
        };
        
        class GLFramebuffer : public GLObject{
        public:
            GLFramebuffer(GLContext * context,GLuint value);
            Ker_CLASS(GLFramebuffer, GLObject, "GLFramebuffer")
        protected:
            virtual void onRecycle();
        };
        
        class GLRenderbuffer : public GLObject{
        public:
            GLRenderbuffer(GLContext * context,GLuint value);
            Ker_CLASS(GLRenderbuffer, GLObject, "GLRenderbuffer")
        protected:
            virtual void onRecycle();
        };
        
        class GLContext : public kk::Object {
        public:
            GLContext();
            virtual ~GLContext();
            
            
            virtual kk::Strong<GLBuffer> createBuffer();
            virtual kk::Strong<GLFramebuffer> createFramebuffer();
            virtual kk::Strong<GLProgram> createProgram();
            virtual kk::Strong<GLRenderbuffer> createRenderbuffer();
            virtual kk::Strong<GLShader> createShader(GLenum type);
            virtual kk::Strong<GLTexture> createTexture();
            
            virtual Native * native();
            virtual void setNative(Native * native);
            virtual void removeObject(GLObject * object);
            virtual void addObject(GLObject * object);
            
            virtual void setFramebuffer(GLuint framebuffer);
            virtual void set(kk::Uint width,kk::Uint height);
            
            Ker_CLASS(GLContext, Object, "GLContext")
            
        protected:
            kk::Uint _width;
            kk::Uint _heigth;
            GLuint _framebuffer;
            kk::Strong<NativeObject> _native;
            std::set<GLObject*> _objects;
        };
        
    }
    
}

#endif /* GLContext_h */
