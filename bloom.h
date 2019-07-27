#pragma once
#include "common.h"

// hide the shader names
namespace impl {
    class GaussianPassShader : public CGLShader {
        GLuint _resolution, _tex;
    public:
        GaussianPassShader(const char* frag_shader) : CGLShader(nullptr, frag_shader) {
            _resolution = glGetUniformLocation(programObject, "resolution");
            _tex = glGetUniformLocation(programObject, "tex");
        };
        void update(vec3 resolution, GLuint tex) {
            glUniform3f(_resolution, resolution.x, resolution.y, resolution.z);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, tex);
            glUniform1i(_tex, 0);
        }
    };

    class GenMipmapShader : public CGLShader {
        GLuint _resolution, _tex;
    public:
        GenMipmapShader(const char* frag_shader) : CGLShader(nullptr, frag_shader) {
            _resolution = glGetUniformLocation(programObject, "resolution");
            _tex = glGetUniformLocation(programObject, "tex");
        };
        void update(vec3 resolution, GLuint tex) {
            glUniform3f(_resolution, resolution.x, resolution.y, resolution.z);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, tex);
            glUniform1i(_tex, 0);
        }
    };

    class BloomShader : public CGLShader {
        GLuint _resolution, _tex_origin, _tex_mipmap;
    public:
        BloomShader(const char* frag_shader) : CGLShader(nullptr, frag_shader) {
            _resolution = glGetUniformLocation(programObject, "resolution");
            _tex_origin = glGetUniformLocation(programObject, "tex_origin");
            _tex_mipmap = glGetUniformLocation(programObject, "tex_mipmap");
        };
        void update(vec3 resolution, GLuint tex_origin, GLuint tex_mipmap) {
            glUniform3f(_resolution, resolution.x, resolution.y, resolution.z);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, tex_origin);
            glUniform1i(_tex_origin, 0);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, tex_mipmap);
            glUniform1i(_tex_mipmap, 1);
        }
    };
}

// 绘制辉光效果
class BloomEffect : public Effect {

    GLuint
        bloom_mipmap_framebuffer = 0,
        bloom_gaussian_h_framebuffer = 0,
        bloom_gaussian_v_framebuffer = 0,
        bloom_out_framebuffer = 0;

    GLuint
        bloom_mipmap_framebuffer_tex = 0,
        bloom_gaussian_h_framebuffer_tex = 0,
        bloom_gaussian_v_framebuffer_tex = 0,
        bloom_out_framebuffer_tex = 0;

    impl::GenMipmapShader* bloom_gen_mipmap_shader;
    impl::GaussianPassShader* bloom_gaussian_h_shader, * bloom_gaussian_v_shader;
    impl::BloomShader* bloom_shader;

    void bloom_framebuffer_init() {
        create_framebuffer(&bloom_mipmap_framebuffer, &bloom_mipmap_framebuffer_tex, resolution_2i().x, resolution_2i().y);
        create_framebuffer(&bloom_gaussian_h_framebuffer, &bloom_gaussian_h_framebuffer_tex, resolution_2i().x, resolution_2i().y);
        create_framebuffer(&bloom_gaussian_v_framebuffer, &bloom_gaussian_v_framebuffer_tex, resolution_2i().x, resolution_2i().y);
        create_framebuffer(&bloom_out_framebuffer, &bloom_out_framebuffer_tex, resolution_2i().x, resolution_2i().y);
    }

public:
    BloomEffect() {
        bloom_framebuffer_init();
        bloom_gen_mipmap_shader = new impl::GenMipmapShader("shader/gen_mipmap.glslf");
        bloom_gaussian_h_shader = new impl::GaussianPassShader("shader/gaussian_pass_h.glslf");
        bloom_gaussian_v_shader = new impl::GaussianPassShader("shader/gaussian_pass_v.glslf");
        bloom_shader = new impl::BloomShader("shader/bloom.glslf");
    }
    void resize() {
        bloom_framebuffer_init();
    }
    GLuint render_texture(GLuint tex_origin) {

        // 生成mipmap
        glBindFramebuffer(GL_FRAMEBUFFER, bloom_mipmap_framebuffer);
        {
            auto shader = use_shader(bloom_gen_mipmap_shader);
            shader->update(resolution_3f(), tex_origin);
            glRectf(-1.0, -1.0, 1.0, 1.0);
        }

        // 横向高斯模糊
        glBindFramebuffer(GL_FRAMEBUFFER, bloom_gaussian_h_framebuffer);
        {
            auto shader = use_shader(bloom_gaussian_h_shader);
            shader->update(resolution_3f(), bloom_mipmap_framebuffer_tex);
            glRectf(-1.0, -1.0, 1.0, 1.0);
        }

        // 纵向高斯模糊
        glBindFramebuffer(GL_FRAMEBUFFER, bloom_gaussian_v_framebuffer);
        {
            auto shader = use_shader(bloom_gaussian_v_shader);
            shader->update(resolution_3f(), bloom_gaussian_h_framebuffer_tex);
            glRectf(-1.0, -1.0, 1.0, 1.0);
        }

        // 采样输出
        glBindFramebuffer(GL_FRAMEBUFFER, bloom_out_framebuffer);
        {
            auto shader = use_shader(bloom_shader);
            shader->update(resolution_3f(), tex_origin, bloom_gaussian_v_framebuffer_tex);
            glRectf(-1.0, -1.0, 1.0, 1.0);
        }

        return bloom_out_framebuffer_tex;
    }
};