//
//  GLContext.c
//  Ker
//
//  Created by zhanghailong on 2019/2/12.
//  Copyright © 2019 kkmofang.cn. All rights reserved.
//

#include "GLContext.h"

namespace kk {
    
    namespace GL {
     
        GLObject::GLObject(GLContext * context,GLuint value):_value(value),_context(context) {
            if(_context != nullptr) {
                _context->addObject(this);
            }
        }
        
        GLObject::~GLObject() {
            if(_context) {
                onRecycle();
                _context->removeObject(this);
            }
        }
        
        void GLObject::recycle() {
            _context = nullptr;
        }
        
        void GLObject::onRecycle() {
            
        }
        
        GLuint GLObject::value() {
            return _value;
        }
        
        kk::Boolean GLObject::isValid() {
            return _context != nullptr;
        }
        
        GLBuffer::GLBuffer(GLContext * context,GLuint value):GLObject(context,value) {
            
        }
        
        void GLBuffer::onRecycle() {
            glDeleteBuffers(1, &_value);
        }
        
        GLTexture::GLTexture(GLContext * context,GLuint value):GLObject(context,value) {
            
        }
        
        void GLTexture::onRecycle() {
            glDeleteTextures(1, &_value);
        }
        
        GLProgram::GLProgram(GLContext * context,GLuint value):GLObject(context,value) {
            
        }
        
        void GLProgram::onRecycle() {
            glDeleteProgram(_value);
        }
        
        GLShader::GLShader(GLContext * context,GLuint value):GLObject(context,value) {
            
        }
        
        void GLShader::onRecycle() {
            glDeleteShader(_value);
        }
        
        GLFramebuffer::GLFramebuffer(GLContext * context,GLuint value):GLObject(context,value) {
            
        }
        
        void GLFramebuffer::onRecycle() {
            glDeleteFramebuffers(1, &_value);
        }
        
        GLRenderbuffer::GLRenderbuffer(GLContext * context,GLuint value):GLObject(context,value) {
            
        }
        
        void GLRenderbuffer::onRecycle() {
            glDeleteRenderbuffers(1, &_value);
        }
        
        
        GLContext::GLContext():_width(0),_heigth(0),_framebuffer(0) {
            
        }
        
        GLContext::~GLContext() {
            {
                auto i = _objects.begin();
                auto e = _objects.end();
                while(i != e) {
                    (*i)->recycle();
                    i ++;
                }
            }
            _native = nullptr;
        }
  
        void GLContext::setFramebuffer(GLuint framebuffer) {
            _framebuffer = framebuffer;
        }
        
        void GLContext::set(kk::Uint width,kk::Uint height) {
            _width = width;
            _heigth = height;
        }
        
        void GLContext::addObject(GLObject * object) {
            _objects.insert(object);
        }
        
        void GLContext::removeObject(GLObject * object) {
            auto i = _objects.find(object);
            if(i != _objects.end()) {
                _objects.erase(i);
            }
        }
        
        Native * GLContext::native() {
            if(_native == nullptr) {
                return nullptr;
            }
            return _native->native();
        }
        
        void GLContext::setNative(Native * native) {
            if(native == nullptr) {
                _native = nullptr;
            } else {
                _native = new NativeObject(native);
            }
        }
        
        kk::Strong<GLBuffer> GLContext::createBuffer() {
            GLuint v = 0;
            glGenBuffers(1, &v);
            return new GLBuffer(this,v);
        }
        
        kk::Strong<GLFramebuffer> GLContext::createFramebuffer() {
            GLuint v = 0;
            glGenFramebuffers(1, &v);
            return new GLFramebuffer(this,v);
        }
        
        kk::Strong<GLProgram> GLContext::createProgram() {
            GLuint v = glCreateProgram();
            return new GLProgram(this,v);
        }
        
        kk::Strong<GLRenderbuffer> GLContext::createRenderbuffer() {
            GLuint v = 0;
            glGenRenderbuffers(1, &v);
            return new GLRenderbuffer(this,v);
        }
        
        kk::Strong<GLShader> GLContext::createShader(GLenum type) {
            GLuint v = glCreateShader(type);
            return new GLShader(this,v);
        }
        
        kk::Strong<GLTexture> GLContext::createTexture() {
            GLuint v = 0;
            glGenTextures(1, &v);
            return new GLTexture(this,v);
        }
        
        
    }
}
