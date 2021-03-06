#include <stdint.h>
#include <assert.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

typedef uint32_t u32;
typedef int32_t i32;

#define WinWidth 1920
#define WinHeight 1080

int main (int ArgCount, char **Args)
{
    u32 WindowFlags = SDL_WINDOW_OPENGL;
    SDL_Window *Window = SDL_CreateWindow("OpenGL Test", 0, 0, WinWidth, WinHeight, WindowFlags);
    assert(Window);
    SDL_GLContext Context = SDL_GL_CreateContext(Window);

    i32 Running = 1;
    i32 FullScreen = 0;

    GLuint VBO;
    GLfloat vertices[] = {
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            0.0f,  0.5f, 0.0f
    };

    while (Running){
        SDL_Event Event;
        while (SDL_PollEvent(&Event)){
            if (Event.type == SDL_KEYDOWN){
                switch (Event.key.keysym.sym){
                    case SDLK_ESCAPE:
                        Running = 0;
                        break;
                    case 'f':
                        FullScreen = !FullScreen;
                        if (FullScreen){
                            SDL_SetWindowFullscreen(Window, WindowFlags | SDL_WINDOW_FULLSCREEN_DESKTOP);
                        }
                        else{
                            SDL_SetWindowFullscreen(Window, WindowFlags);
                        }
                        break;
                    default:
                        break;
                }
            }
            else if (Event.type == SDL_QUIT){
                Running = 0;
            }
        }
        glViewport(0, 0, WinWidth, WinHeight);

        glClearColor(1.f, 0.f, 1.f, 0.f);
        glClear(GL_COLOR_BUFFER_BIT);

        SDL_GL_SwapWindow(Window);
    }
    return 0;
}