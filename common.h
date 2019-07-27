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

// �������ȵ�ƽ��
inline float length2(vec3 v) { return dot(v, v); }

// ���ڴ�С
i32vec2 resolution_2i();

// ���ڴ�С
vec3 resolution_3f();

// ���������ʱ��
float time();

// ����һ������
void load_texture(GLuint* tex_id, char* filename);

void load_cubemap(GLuint* tex_id, const vector<char*>& files);

// ����֡������
void create_framebuffer(GLuint* framebuffer, GLuint* framebuffer_tex, int w, int h);

// ���ô�С
// �ڴ�����״���ı�ʱ����
void _set_geometry(int x, int y, int z, int w);

// ���ÿ�ʼ��ʱ
// �ڳ�������ʱ����
void _set_time_start();

// ����һ��ͼ��Ч��
class Effect {
public:
    virtual void resize() {};
};

// ������������۵�����ģ��
// ���ڼ���ڶ���������ļ��ٶ�
class SchwarzschildMetric {
    float _mass;
public:
    float mass() { return _mass; }
    float r_event_horizon() { return 2 * _mass; }
    float r_photon_sphere() { return 1.5* r_event_horizon(); }
    float r_isco() { return 3 * r_event_horizon(); };

    // ��û���Թ�
    vec3 get_accel(vec3 r, vec3 v) {
        vec3 c = cross(r, v);
        float d�� = length(c) / length2(r);
        float dr = length2(r) / length2(r);
        float dd�� = -2 * dr * d�� / length(r);
        float ddr = -_mass / length2(r) + (length(r) - 3 * _mass) * powf(d��, 2);
        vec3 plane = normalize(c);
        vec3 basis_x = normalize(r);
        vec3 basis_y = cross(plane, basis_x);
        float acc_x = ddr - length(r) * powf(d��, 2);
        float acc_y = length(r) * dd�� + 2 * dr + d��;
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