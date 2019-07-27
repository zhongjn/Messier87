#pragma once
#include "common.h"

// hide the shader names
namespace impl {
    class DrawTextureShader : public CGLShader {
        GLuint _resolution, _tex;
    public:
        DrawTextureShader(const char* frag_shader) : CGLShader(nullptr, frag_shader) {
            _resolution = glGetUniformLocation(programObject, "resolution");
            _tex = glGetUniformLocation(programObject, "tex_in");
        };
        void update(vec3 resolution, GLuint tex) {
            glUniform3f(_resolution, resolution.x, resolution.y, resolution.z);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, tex);
            glUniform1i(_tex, 0);
        }
    };
}

// 绘制一个纹理，使其充满屏幕
class DrawTextureEffect : public Effect {
    impl::DrawTextureShader* draw_texture_shader;

public:
    DrawTextureEffect() {
        draw_texture_shader = new impl::DrawTextureShader("shader/draw_tex.glslf");
    }

    void draw(GLuint texture) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        auto shader = use_shader(draw_texture_shader);
        shader->update(resolution_3f(), texture);
        glRectf(-1, -1, 1, 1);
    }
};