#pragma once
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <iostream>
#include <Windows.h>
// #define GLUT_DISABLE_ATEXIT_HACK 

#include <vector>
#include <string>
#include <stdarg.h>
#include <memory>

#include "gl/glew.h"
#include "gl/glut.h"
#include "glm/glm.hpp"
#include "glm/vec3.hpp"
#include "cgl_shader.h"

using namespace std;
using namespace glm;

// 向量长度的平方
inline float length2(vec3 v) { return dot(v, v); }

// 窗口大小
i32vec2 resolution_2i();

// 窗口大小
vec3 resolution_3f();

// 启动至今的时间
float time();

// 加载一个纹理
void load_texture(GLuint* tex_id, char* filename);

void load_cubemap(GLuint* tex_id, const vector<char*>& files);

// 创建帧缓冲区
void create_framebuffer(GLuint* framebuffer, GLuint* framebuffer_tex, int w, int h);

// 设置大小
// 在窗口形状被改变时调用
void _set_geometry(int x, int y, int z, int w);

// 设置开始计时
// 在程序启动时调用
void _set_time_start();

// 代表一个图形效果
class Effect {
public:
    virtual void resize() {};
};

// 包含广义相对论的物理模型
// 用于计算黑洞附近物体的加速度
class SchwarzschildMetric {
    float _mass;
public:
    float mass() { return _mass; }
    float r_event_horizon() { return 2 * _mass; }
    float r_photon_sphere() { return 1.5* r_event_horizon(); }
    float r_isco() { return 3 * r_event_horizon(); };

    // 还没测试过
    vec3 get_accel(vec3 r, vec3 v) {
        vec3 c = cross(r, v);
        float dθ = length(c) / length2(r);
        float dr = length2(r) / length2(r);
        float ddθ = -2 * dr * dθ / length(r);
        float ddr = -_mass / length2(r) + (length(r) - 3 * _mass) * powf(dθ, 2);
        vec3 plane = normalize(c);
        vec3 basis_x = normalize(r);
        vec3 basis_y = cross(plane, basis_x);
        float acc_x = ddr - length(r) * powf(dθ, 2);
        float acc_y = length(r) * ddθ + 2 * dr + dθ;
        return basis_x * acc_x + basis_y * acc_y;
    }

    SchwarzschildMetric(float mass) : _mass(mass) {}
};

inline string string_format(const string fmt_str, ...) {
    int final_n, n = ((int)fmt_str.size()) * 2; /* Reserve two times as much as the length of the fmt_str */
    unique_ptr<char[]> formatted;
    va_list ap;
    while (1) {
        formatted.reset(new char[n]); /* Wrap the plain char array into the unique_ptr */
        strcpy(&formatted[0], fmt_str.c_str());
        va_start(ap, fmt_str);
        final_n = vsnprintf(&formatted[0], n, fmt_str.c_str(), ap);
        va_end(ap);
        if (final_n < 0 || final_n >= n)
            n += abs(final_n - n + 1);
        else
            break;
    }
    return string(formatted.get());
}
//
//template<typename T>
//inline void bound(T& value, T from, T to) {
//    if (value > from) value = from;
//    if ()
//}