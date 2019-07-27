#pragma once
#include "common.h"
#include "glm/gtc/type_ptr.hpp"

// hide the shader names
namespace impl {
    class BlackholeShader : public CGLShader {
        GLint _resolution, _time, _tex_disc, _tex_previous, _tex_sun, _tex_bg, _cam_mat, _samples;
    public:
        BlackholeShader(const char* frag_shader) : CGLShader(nullptr, frag_shader) {
            _resolution = glGetUniformLocation(programObject, "resolution");
            _time = glGetUniformLocation(programObject, "time_total");
            _tex_disc = glGetUniformLocation(programObject, "tex_disc");
            _tex_previous = glGetUniformLocation(programObject, "tex_previous");
            _tex_sun = glGetUniformLocation(programObject, "tex_sun");
            _tex_bg = glGetUniformLocation(programObject, "tex_bg");
            _cam_mat = glGetUniformLocation(programObject, "cam_mat");
            _samples = glGetUniformLocation(programObject, "samples");
        };
        void update(vec3 resolution, float time, GLuint tex_disc, GLuint tex_previous, GLuint tex_sun, GLuint tex_bg, const mat4& cam_mat, int samples) {
            glUniform3f(_resolution, resolution.x, resolution.y, resolution.z);
            glUniform1f(_time, time);

            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, tex_disc);
            glUniform1i(_tex_disc, 2);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, tex_previous);
            glUniform1i(_tex_previous, 1);

            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, tex_sun);
            glUniform1i(_tex_sun, 3);

            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, tex_bg);
            glUniform1i(_tex_bg, 4);

            glUniformMatrix4fv(_cam_mat, 1, GL_FALSE, glm::value_ptr(cam_mat));

            glUniform1i(_samples, samples);
        }
    };
}

// 绘制黑洞
class BlackholeEffect : public Effect {
    GLuint framebuffer = 0;
    GLuint framebuffer_tex = 0;
    GLuint tex_disc = 0, tex_sun = 0, tex_bg;
    impl::BlackholeShader* blackhole_shader = nullptr;
    void blackhole_framebuffer_init() {
        create_framebuffer(&framebuffer, &framebuffer_tex, resolution_2i().x, resolution_2i().y);
    }
public:
    BlackholeEffect() {
        blackhole_framebuffer_init();
        load_texture(&tex_disc, "texture/adisk2.bmp");
        load_texture(&tex_sun, "texture/sun.bmp");
        load_texture(&tex_bg, "texture/milky_way_nasa.bmp");
        blackhole_shader = new impl::BlackholeShader("shader/blackhole_adisc.glslf");
    }

    // 窗口改变大小时调用
    // 重新创建帧缓冲区
    void resize() {
        blackhole_framebuffer_init();
    }

    // 将黑洞绘制到纹理上
    // 返回纹理id
    GLuint render_texture(const mat4& cam_mat, int samples) {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        auto shader = use_shader(blackhole_shader);
        shader->update(resolution_3f(), time(), tex_disc, framebuffer_tex, tex_sun, tex_bg, cam_mat, samples);
        glRectf(-1.0, -1.0, 1.0, 1.0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return framebuffer_tex;
    }
};