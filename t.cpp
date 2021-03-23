#define GL_GLEXT_PROTOTYPES

#include <iostream>
#include <SDL2/SDL.h>
#include <vector>
#include <SDL2/SDL_opengl.h>
#include <stdio.h>
#include "objects.h"
#include <cmath>
#include <sys/time.h>
#include <thread>

using namespace std;

typedef float t_mat4x4[16];

static inline void
mat4x4_ortho(t_mat4x4 out, float left, float right, float bottom, float top, float znear, float zfar) {
#define T(a, b) (a * 4 + b)

    out[T(0, 0)] = 2.0f / (right - left);
    out[T(0, 1)] = 0.0f;
    out[T(0, 2)] = 0.0f;
    out[T(0, 3)] = 0.0f;

    out[T(1, 1)] = 2.0f / (top - bottom);
    out[T(1, 0)] = 0.0f;
    out[T(1, 2)] = 0.0f;
    out[T(1, 3)] = 0.0f;

    out[T(2, 2)] = -2.0f / (zfar - znear);
    out[T(2, 0)] = 0.0f;
    out[T(2, 1)] = 0.0f;
    out[T(2, 3)] = 0.0f;

    out[T(3, 0)] = -(right + left) / (right - left);
    out[T(3, 1)] = -(top + bottom) / (top - bottom);
    out[T(3, 2)] = -(zfar + znear) / (zfar - znear);
    out[T(3, 3)] = 1.0f;

#undef T
}

const float MIN_DIST = 1;
const float MAX_DIST = 50;
const int MAX_STEP = 10;
const float angle = 67. / 180. * M_PI;

const vec3 lightPos{0, 10, 0};
const vec3 lightColor{1, 1, 1};
camera cam;

vec3 **directions;

typedef enum t_attrib_id {
    attrib_position,
    attrib_color
} t_attrib_id;

std::vector<SDFObject *> objects;

vec3 getNormal(const SDFObject &obj, const vec3 &pos) {
    float dt = 1e-5;
    float dx = obj.sdf(pos.x + dt, pos.y, pos.z) - obj.sdf(pos.x - dt, pos.y, pos.z);
    float dy = obj.sdf(pos.x, pos.y + dt, pos.z) - obj.sdf(pos.x, pos.y - dt, pos.z);
    float dz = obj.sdf(pos.x, pos.y, pos.z + dt) - obj.sdf(pos.x, pos.y, pos.z - dt);
    return vec3(dx, dy, dz).normalize();
}

vec3 getColor(const vec3 &dir) {

    float md = 10000;
    float len = 0;
    int step = 0;

    vec3 pos(cam.pos);
    vec3 c;
    SDFObject *o;

    while (len < MAX_DIST && step < MAX_STEP) {

        for (SDFObject *obj : objects) {
            float r = obj->sdf(pos);
            if (r < md) {
                md = r;
                c = obj->getColor(pos);
                o = obj;
            }
        }

        pos += md * dir;
        if (md < MIN_DIST)
            break;
        len += md;
        step++;
    }
    if (md > MIN_DIST && (len >= MAX_DIST || step >= MAX_STEP)) {
        return c.set(0, .6, .6);
    } else {
        float ambientStrength = 0.4;
        float specularStrength = 0.1;
        float reflectionStrength = 2;

        vec3 N = getNormal(*o, pos);
        vec3 L = (lightPos - pos).normalize();
        vec3 V = (cam.pos - pos).normalize();
        vec3 R = reflect(-L, N).normalize();
        vec3 T = (2 * outProd(N, V) * N - V).normalize();

        float diff = max(inProd(N, L), 0.0f);

        vec3 ambient = ambientStrength * lightColor;
        vec3 diffuse = diff * lightColor;
        vec3 specular = (float) pow(max(inProd(V, R), 0.0f), 32) * specularStrength * lightColor;
        c = (ambient + diffuse + specular) * (c);
    }

    return c * (1 - len / (2 * MAX_DIST));
}

void update(GLfloat *colors, int w, int h,const mat3& m) {
    for (int z = 0; z < h; ++z) {
        for (int y = 0; y < w; ++y) {
            vec3 color = getColor(m*(directions[y][z]));
            int index = 4 * (y * h + z);
            colors[index] = color.x;
            colors[index + 1] = color.y;
            colors[index + 2] = color.z;
            colors[index + 3] = 1;
        }
    }
}

void sdl_init() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

}

int shader_init(GLuint &idVertexShader, GLuint &idFragmentShader) {
    const char *vertex_shader =
            "#version 130\n"
            "in vec2 i_position;\n"
            "in vec4 i_color;\n"
            "out vec4 v_color;\n"
            "uniform mat4 u_projection_matrix;\n"
            "void main() {\n"
            "    v_color = i_color;\n"
            "    gl_Position = u_projection_matrix*vec4( i_position, 0.0, 1.0 );\n"
            "}\n";

    const char *fragment_shader =
            "#version 130\n"
            "in vec4 v_color;\n"
            "out vec4 o_color;\n"
                    "void main() {\n"
            "    o_color = v_color;\n"
            "}\n";

    idVertexShader = glCreateShader(GL_VERTEX_SHADER);
    idFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    int length = strlen(vertex_shader);
    glShaderSource(idVertexShader, 1, (const GLchar **) &vertex_shader, &length);
    glCompileShader(idVertexShader);

    GLint status;
    glGetShaderiv(idVertexShader, GL_COMPILE_STATUS, &status);

    if (status == GL_FALSE) {
        fprintf(stderr, "vertex shader compilation failed\n");
        return 1;
    }

    length = strlen(fragment_shader);
    glShaderSource(idFragmentShader, 1, (const GLchar **) &fragment_shader, &length);
    glCompileShader(idFragmentShader);

    glGetShaderiv(idFragmentShader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        fprintf(stderr, "fragment shader compilation failed\n");
        return 1;
    }

}

int main() {

    sdl_init();


    mat3 m;
    m.idt();

    const int width = 800;
    const int height = 600;

    directions = new vec3 *[width];
    const auto d = (float) (height / 2. / tan(angle / 2));
    for (int i = 0; i < width; i++) {
        directions[i] = new vec3[height];
        for (int j = 0; j < height; j++) {
            directions[i][j].z = d;
            directions[i][j].x = (float) (i - width / 2.0);
            directions[i][j].y = (float) (j - height / 2.0);
            directions[i][j].normalize();
        }
    }

    uint32_t windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
    SDL_Window *window = SDL_CreateWindow("start window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height,
                                          windowFlags);
    SDL_GLContext context = SDL_GL_CreateContext(window);

    GLuint
            idVertexShader,
            idFragmentShader,
            idProgram;

    if (!shader_init(idVertexShader, idFragmentShader))
        return 1;

    idProgram = glCreateProgram();
    glAttachShader(idProgram, idVertexShader);
    glAttachShader(idProgram, idFragmentShader);

    glBindAttribLocation(idProgram, attrib_position, "i_position");
    glBindAttribLocation(idProgram, attrib_color, "i_color");

    glLinkProgram(idProgram);
    glUseProgram(idProgram);

    glDisable(GL_DEPTH_TEST);

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glViewport(0, 0, width, height);

    auto *colors_buffer_data = new GLfloat[4 * width * height]{0};
    auto *positions_buffer_data = new GLfloat[2 * width * height]{0};

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            int index = (x * height + y) * 2;
            positions_buffer_data[index] = (float) (x);
            positions_buffer_data[index + 1] = (float) (height - y);
        }
    }

    vec3 point(0,0,10);

    SDFSphere s1(00, 0, 10, 1);
    s1.color.set(1, 0, 0);
    SDFSphere s2(4, 0, 10, 0.5f);
    s2.color.set(1, 1, 0);

    SDFSphere2 ss(s1,s2);

    objects.push_back(&s1);
    //objects.push_back(&ss);

    SDFPlane p1(0, 0, -2, 0, 0, 1);
    p1.color.set(0, 0, 1);

    objects.push_back(&p1);

    //update(colors_buffer_data, width, height,m);

    GLuint
            idVertexArray,
            idPositionBuffer,
            idColorBuffer;


    glGenVertexArrays(1, &idVertexArray);
    glGenBuffers(1, &idPositionBuffer);
    glGenBuffers(1, &idColorBuffer);

    glBindVertexArray(idVertexArray);

    //positions
    glBindBuffer(GL_ARRAY_BUFFER, idPositionBuffer);

    glVertexAttribPointer(attrib_position, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, nullptr);
    glBufferData(GL_ARRAY_BUFFER, width * height * 2 * sizeof(GLfloat), positions_buffer_data, GL_STATIC_DRAW);
    glEnableVertexAttribArray(attrib_position);

    //colors
    glBindBuffer(GL_ARRAY_BUFFER, idColorBuffer);

    glBufferData(GL_ARRAY_BUFFER, width * height * 4 * sizeof(GLfloat), colors_buffer_data, GL_STREAM_DRAW);
    glVertexAttribPointer(attrib_color, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, nullptr);
    glEnableVertexAttribArray(attrib_color);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    t_mat4x4 projection_matrix;
    mat4x4_ortho(projection_matrix, 0.0f, (float) width, (float) height, 0.0f, 0.0f, 100.0f);
    glUniformMatrix4fv(glGetUniformLocation(idProgram, "u_projection_matrix"), 1, GL_FALSE, projection_matrix);

    int run = 1;
    int FullScreen = windowFlags & SDL_WINDOW_FULLSCREEN_DESKTOP;

    timeval time_now{};
    time_t timer;

    mat3 i_m ;

    while (run) {
        SDL_Event Event;
        while (SDL_PollEvent(&Event)) {
            if (Event.type == SDL_KEYDOWN) {
                switch (Event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        run = 0;
                        break;
                    case 'f':
                        FullScreen = !FullScreen;
                        if (FullScreen)
                            SDL_SetWindowFullscreen(window, windowFlags | SDL_WINDOW_FULLSCREEN_DESKTOP);
                        else
                            SDL_SetWindowFullscreen(window, windowFlags);
                        break;
                    default:
                        break;
                }
            } else if (Event.type == SDL_QUIT)
                run = 0;
        }


        float x = cam.pos.x;
        float z = cam.pos.z;
        float a = 0.1f;
        cam.pos.x = point.x +  (float)(x - point.x)*cos(a) - (float)(z - point.z) * sin(a);
        cam.pos.z = point.z +  (float)(x - point.x)*sin(a) + (float)(z - point.z) * cos(a);

        cam.look(point);

        vec3 ox= outProd(cam.up,cam.dir);
        m.set(
                ox.x,ox.y,ox.z,
                cam.up.x,cam.up.y,cam.up.z,
                cam.dir.x,cam.dir.y,cam.dir.z
                );
        m.inv();
        std::cout << "start update\n";
        gettimeofday(&time_now, nullptr);
        timer = (time_now.tv_sec * 1000) + (time_now.tv_usec / 1000);

        update(colors_buffer_data, width, height,m);

        gettimeofday(&time_now, nullptr);
        timer = (time_now.tv_sec * 1000) + (time_now.tv_usec / 1000) - timer;
        std::cout << "end update:" << timer << '\n';

        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(idProgram);

        glBindVertexArray(idVertexArray);

        glBindBuffer(GL_ARRAY_BUFFER, idColorBuffer);
        glBufferData(GL_ARRAY_BUFFER, width * height * 4 * sizeof(GLfloat), colors_buffer_data, GL_STREAM_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_POINTS, 0, width * height);

        glBindVertexArray(0);

        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}