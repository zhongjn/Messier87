#include "common.h"

double geometry[4] = { 800, 600, 0, 0 };
clock_t tick_start;

i32vec2 resolution_2i() {
    return i32vec2(geometry[0], geometry[1]);
}

vec3 resolution_3f() {
    return vec3(geometry[0], geometry[1], 1);
}

float time() {
    return float(clock() - tick_start) / 1000;
}

void load_texture(GLuint* tex_id, char* filename)
{
    HBITMAP hBMP;
    BITMAP BMP;
    hBMP = (HBITMAP)LoadImage(NULL, filename, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);
    GetObject(hBMP, sizeof(BMP), &BMP);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glGenTextures(1, tex_id);
    glBindTexture(GL_TEXTURE_2D, *tex_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, BMP.bmWidth, BMP.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, BMP.bmBits);

    DeleteObject(hBMP);
}

void load_cubemap(GLuint* tex_id, const vector<char*>& files)
{
    if (files.size() != 6) throw logic_error("expect 6 files");
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glGenTextures(1, tex_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, *tex_id);
    for (int i = 0; i < 6; i++) {
        char* filename = files[i];
        HBITMAP hBMP;
        BITMAP BMP;
        hBMP = (HBITMAP)LoadImage(NULL, filename, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);
        GetObject(hBMP, sizeof(BMP), &BMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0, 3, BMP.bmWidth, BMP.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, BMP.bmBits
        );
    }
}

void create_framebuffer(GLuint* framebuffer, GLuint* framebuffer_tex, int w, int h) {
    GLuint fb;
    glGenFramebuffers(1, &fb);
    glBindFramebuffer(GL_FRAMEBUFFER, fb);
    // generate texture
    GLuint texColorBuffer;
    glGenTextures(1, &texColorBuffer);
    glBindTexture(GL_TEXTURE_2D, texColorBuffer);

    // i32vec2 res = resolution();

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // attach it to currently bound framebuffer object
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

    glDeleteFramebuffers(1, framebuffer);
    glDeleteTextures(1, framebuffer_tex);

    *framebuffer = fb;
    *framebuffer_tex = texColorBuffer;
}

void _set_geometry(int x, int y, int z, int w) {
    geometry[0] = x;
    geometry[1] = y;
    geometry[2] = z;
    geometry[3] = w;
}

void _set_time_start() {
    tick_start = clock();
}