#pragma once
// #define GLUT_DISABLE_ATEXIT_HACK 
#include "gl/glew.h"
#include <iostream>

// ��װһ��GLSL��ɫ����
// �γ�Դ����������
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


// RAII�������һ����ɫ������ʱʹ��
// ����ʱ����enable()������ʱ����disable()
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

// ��װCGLShaderUsage�Ĺ��캯����ʹ����ڵ���
template<typename Ptr>
inline CGLShaderUsage<Ptr> use_shader(Ptr shader) {
    return CGLShaderUsage<Ptr>(shader);
}