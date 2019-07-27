#pragma once
// #define GLUT_DISABLE_ATEXIT_HACK 
#include "gl/glew.h"
#include <iostream>

// 封装一个GLSL着色管线
// 课程源码里搬过来的
class CGLShader {
protected:
    GLhandleARB programObject;

public:
    CGLShader(const char *vfile, const char *ffile);
    virtual ~CGLShader();

    virtual void enable();
    virtual void disable();
    // virtual void update();

private:
    void cleanExit(int exitval);
    void printInfoLog(GLhandleARB object);
    void addShader(const GLcharARB *shaderSource, GLenum shaderType);
};


// RAII，代表对一个着色器的临时使用
// 构造时调用enable()，析构时调用disable()
template<typename Ptr>
class CGLShaderUsage {
    Ptr _shader;
public:
    CGLShaderUsage(const CGLShaderUsage&) = delete;
    CGLShaderUsage(CGLShaderUsage&& other) {
        if (&other != this) {
            _shader = other._shader;
            other._shader = nullptr;
        }
    }
    CGLShaderUsage(Ptr shader) : _shader(shader) {
        if (_shader) _shader->enable();
    }
    ~CGLShaderUsage() {
        if (_shader) _shader->disable();
    }
    Ptr operator->() {
        return _shader;
    }
};

// 封装CGLShaderUsage的构造函数，使其便于调用
template<typename Ptr>
inline CGLShaderUsage<Ptr> use_shader(Ptr shader) {
    return CGLShaderUsage<Ptr>(shader);
}