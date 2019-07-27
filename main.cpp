#include "common.h"
#include "bloom.h"
#include "draw_texture.h"
#include "blackhole.h"
#include "input.h"
#include <Windows.h>
#include <vector>
#include <unordered_map>

int blackhole_samples_index = 0;
vector<int> blackhole_samples = { 1, 2, 4, 8 };

// ����Ч��
// ���ڴ��ڸı��Сʱ����resize()
vector<Effect*> effects;

BlackholeEffect* ef_blackhole;
BloomEffect* ef_bloom;
DrawTextureEffect* ef_draw_tex;

Input input;

mat4 cam_mat;
vec3 cam_pos = vec3(6.5, 1.4, -14);
vec3 cam_rot = vec3(-.45, 0.078, 0.31); // yaw, pitch, roll
vec3 cam_pos_v;
vec3 cam_rot_v;
float cam_pos_v_init = 2.f;
float cam_rot_v_init = 0.25f;
float cam_v_decay = 0.1;

void set_cam_pos_velocity(vec3 dir) {
    cam_pos_v += vec3(cam_mat * vec4(dir, 0) * cam_pos_v_init);
}

void set_cam_rot_velocity(vec3 rot) {
    cam_rot_v += rot * cam_rot_v_init;
}

void print_string(int x, int y, const string& str) {
    glWindowPos2i(x, y);
    for (int i = 0; i < str.length(); ++i) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, str[i]);
    }
}

void update();
void render();

float time_previous = -1;
float fps = 0;
float time_elapsed;

void compute_time() {
    static int frames, last_time;
    auto ticks = clock();
    if (frames == 0) last_time = ticks;
    frames++;
    if (ticks - last_time >= 500)
    {
        fps = 1000.0 * frames / (ticks - last_time);
        frames = 0;
    }

    if (time_previous < 0) time_previous = time();
    time_elapsed = time() - time_previous;
    time_previous = time();
}

static void display() {
    compute_time();
    update();
    render();

    glFinish();
    glutPostRedisplay();
}


static void reshape(int w, int h) {
    _set_geometry(w, h, 0, 0);
    glViewport(0, 0, w, h);
    for (Effect* ef : effects) {
        ef->resize();
    }
}

static void key_up(unsigned char k, int x, int y)
{
    input.on_key_up(k);
}

static void key_down(unsigned char k, int x, int y)
{
    input.on_key_down(k);
    if (k == 'n') blackhole_samples_index--;
    if (k == 'm') blackhole_samples_index++;
    blackhole_samples_index = clamp<int>(blackhole_samples_index, 0, blackhole_samples.size() - 1);
}

void update() {
    cam_pos += cam_pos_v * time_elapsed;
    cam_rot += cam_rot_v * time_elapsed;

    cam_pos_v *= 1 - cam_v_decay * max(1.f, time_elapsed);
    cam_rot_v *= 1 - cam_v_decay * max(1.f, time_elapsed);

    float factor = input.get_key(' ') ? .05 : 1;

    if (input.get_key('a')) {
        set_cam_pos_velocity(vec3(-1, 0, 0) * factor);
    }
    if (input.get_key('d')) {
        set_cam_pos_velocity(vec3(1, 0, 0) * factor);
    }
    if (input.get_key('w')) {
        set_cam_pos_velocity(vec3(0, 1, 0) * factor);
    }
    if (input.get_key('s')) {
        set_cam_pos_velocity(vec3(0, -1, 0) * factor);
    }
    if (input.get_key('z')) {
        set_cam_pos_velocity(vec3(0, 0, -1) * factor);
    }
    if (input.get_key('x')) {
        set_cam_pos_velocity(vec3(0, 0, 1) * factor);
    }
    if (input.get_key('j')) {
        set_cam_rot_velocity(vec3(-1, 0, 0) * factor);
    }
    if (input.get_key('l')) {
        set_cam_rot_velocity(vec3(1, 0, 0) * factor);
    }
    if (input.get_key('i')) {
        set_cam_rot_velocity(vec3(0, -1, 0) * factor);
    }
    if (input.get_key('k')) {
        set_cam_rot_velocity(vec3(0, 1, 0) * factor);
    }
    if (input.get_key('u')) {
        set_cam_rot_velocity(vec3(0, 0, -1) * factor);
    }
    if (input.get_key('o')) {
        set_cam_rot_velocity(vec3(0, 0, 1) * factor);
    }

    cam_mat = glm::identity<mat4>();
    cam_mat = glm::translate(cam_mat, cam_pos);
    cam_mat = glm::rotate(cam_mat, cam_rot[0], vec3(0, 1, 0));
    cam_mat = glm::rotate(cam_mat, cam_rot[1], vec3(1, 0, 0));
    cam_mat = glm::rotate(cam_mat, cam_rot[2], vec3(0, 0, 1));
}

void render() {
    int samples = blackhole_samples[blackhole_samples_index];
    GLuint tex_bh = ef_blackhole->render_texture(cam_mat, samples);
    GLuint tex_bh_bloom = ef_bloom->render_texture(tex_bh);
    ef_draw_tex->draw(tex_bh_bloom);

    int pos_y = 0;
    print_string(15, pos_y += 15, string_format("please read the instructions in the console window"));
    print_string(15, pos_y += 15, string_format("R=%.2fRs", length(cam_pos)));
    print_string(15, pos_y += 15, string_format("MSAA=%dX", samples));
    print_string(15, pos_y += 15, string_format("FPS=%.2f", fps));
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_SINGLE);
    glutInitWindowSize(700, 500);

    int windowHandle = glutCreateWindow("Messier87");
    _set_time_start();

    printf("Initializing...\n");
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        printf("Error: %s\n", glewGetErrorString(err));
    }

    // ��ʼ��Ч��
    effects.push_back(ef_blackhole = new BlackholeEffect());
    effects.push_back(ef_bloom = new BloomEffect());
    effects.push_back(ef_draw_tex = new DrawTextureEffect());
    printf("Done.\n");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(key_down);
    glutKeyboardUpFunc(key_up);

    printf("\n");
    printf("0. English Version\n");
    printf("    https://github.com/zhongjn/Messier87/README.md\n");


    printf("\n");
    printf("1. ����ʲô��\n");
    printf("    ����һ�����ڣ����ƣ���������۵�ʷ�����ڶ�ģ����������۲쵽������Ť�����Ǻڶ�Ť���˹�����ɵģ��ⱻ��Ϊ����͸��ЧӦ��\n");

    printf("\n");
    printf("2. ��β�����\n");
    printf("    WASDZX - �ƶ���ͷ\n");
    printf("    IJKLUO - ��ת��ͷ\n");
    printf("    �ո� - ������ͷ�ٶ�\n");
    printf("    NM - ��/������ݱ���\n");

    printf("\n");
    printf("3. ע\n");
    printf("    �������½���ʾ������ڶ��ľ��룬����RsΪ�¼��ӽ�İ뾶��\n");
    printf("    R=3.00Rsʱ���㴦����ڵ��ȶ�������������ڰ뾶����������\n");
    printf("    R=1.50Rsʱ���㴦�ڹ��ӵĻ��ƹ����������ܿ���ˮƽ�ĺڶ���Ӱ������㱾�����������Կ����Լ��ĺ����ס�\n");
    printf("    R=1.00Rsʱ���㴦���¼��ӽ��ϡ���Ϊ����һ�����Ƶ�ģ�⣬���Ի����ڴ�ʱ�Ѿ�ʧ�档ʵ������ǡ�����������������壬��ô����һ���������㲻�ᷢ���Լ��������¼��ӽ磻���������ͼ������Ժڶ���ֹ����ô��ֻ����Զ��ڶ��ķ��򿴵�һ���⡣\n");
    
    printf("\n");
    printf("4. ����\n");
    printf("    https://github.com/zhongjn\n");

    glutMainLoop();
}