#pragma once
#include <GL/glew.h> // Важно: GLEW должен быть ПЕРЕД SDL.h
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <cmath>
#include <cstdio>
#include <functional>
#include "v2.h"
#include <iostream>

using namespace std;

namespace lvichki {



bool dbg_draw = false;

class Game {
private:
    SDL_Window*   window = nullptr;
    
    TTF_Font*     font = nullptr;
    Uint32 fps_start_time = 0;
    Uint32 last_frame_time = 0;
    int    fps_frames = 0;
    bool is_valid = false;



public:
    int width = 1920;
    int height = 1080;
    float dt = 0;
    float fps = 0.0f;
    bool show_fps = true;  // По умолчанию показываем FPS
    const float fixed_dt = 1.0f / 60.0f; // Фиксированный шаг времени для физики
    function<void()> on_update = [](){};
    function<void()> on_draw   = [](){};
    function<void(const SDL_Event&)> on_event = [](const SDL_Event&){};
    uint64_t update_count = 0;
    uint64_t draw_count = 0;

    SDL_GLContext gl_context;
    GLuint shaderProgram;
    GLuint quadVAO, quadVBO;

    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec2 aTexCoord;
        
        uniform mat4 u_mvp;
        out vec2 TexCoord;

        void main() {
            gl_Position = u_mvp * vec4(aPos, 0.0, 1.0);
            TexCoord = aTexCoord;
        }
    )";

    const char* fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;
        in vec2 TexCoord;
        
        uniform sampler2D u_texture;
        uniform vec4 u_color;

        void main() {
            FragColor = texture(u_texture, TexCoord) * u_color;
        }
    )";

    void init_opengl() {
        // 1. Создаем контекст (обязательно ПЕРЕД glewInit)
        gl_context = SDL_GL_CreateContext(window);
        if (!gl_context) {
            SDL_Log("GL Context error: %s", SDL_GetError());
            return;
        }

        // Инициализируем GLEW, чтобы стали доступны функции типа glCreateShader
        glewExperimental = GL_TRUE;
        GLenum err = glewInit();
        if (err != GLEW_OK) {
            SDL_Log("GLEW Error: %s", glewGetErrorString(err));
        }

        // 2. Настраиваем Viewport
        glViewport(0, 0, width, height);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // 3. Компиляция шейдеров (упрощенно)
        auto compile = [](GLenum type, const char* src) {
            GLuint s = glCreateShader(type);
            glShaderSource(s, 1, &src, NULL);
            glCompileShader(s);
            return s;
        };
        GLuint vs = compile(GL_VERTEX_SHADER, vertexShaderSource);
        GLuint fs = compile(GL_FRAGMENT_SHADER, fragmentShaderSource);
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vs);
        glAttachShader(shaderProgram, fs);
        glLinkProgram(shaderProgram);

        // 4. Создаем геометрию квадрата (0,0 to 1,1)
        float vertices[] = { 
            // pos          // tex
            -0.5f,  0.5f,   0.0f, 0.0f,
            0.5f, -0.5f,   1.0f, 1.0f,
            -0.5f, -0.5f,   0.0f, 1.0f,

            -0.5f,  0.5f,   0.0f, 0.0f,
            0.5f,  0.5f,   1.0f, 0.0f,
            0.5f, -0.5f,   1.0f, 1.0f
        };

        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }


    Game() {

        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            SDL_Log("SDL_Init error: %s", SDL_GetError());
            return;
        }

        if (TTF_Init() == -1) {
            SDL_Log("TTF_Init error: %s", TTF_GetError());
        }

        window = SDL_CreateWindow("lvichki",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            width, height, 
            SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

        if (!window) {
            SDL_Log("Window creation error: %s", SDL_GetError());
            return;
        }

        init_opengl();

        // === ШРИФТ ПО УМОЛЧАНИЮ ===
        font = TTF_OpenFont("FreeSans.ttf", 32);
        if (!font) {
            font = TTF_OpenFont("/storage/emulated/0/Download/freesans/FreeSans.ttf", 32);
        }
        if (!font) {
            SDL_Log("Не удалось загрузить шрифт по умолчанию: %s", TTF_GetError());
            SDL_Log("Текст (включая FPS) не будет отображаться.");
        }

        // FPS счётчик
        fps_start_time = SDL_GetTicks();
        last_frame_time = fps_start_time;

        is_valid = true;
    }

    ~Game() {
        if (font) TTF_CloseFont(font);
        if (window)   SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
    }

    void run() {
        if (!is_valid) return;
        bool running = true;
        SDL_Event e;
        
        float accumulator = 0.0f;
        Uint32 last_time = SDL_GetTicks();

        while (running) {
            update_delta_and_fps();
            //dbg_print_frames();
            Uint32 now = SDL_GetTicks();
            float frame_time = (now - last_time) / 1000.0f;
            if (frame_time > 0.25f) frame_time = 0.25f; // "Защита от спирали смерти"
            last_time = now;

            accumulator += frame_time;

            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT) {
                    running = false;
                }
                if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                }
                on_event(e);
            }

            
            // Пока накопилось достаточно времени, крутим физику
            while (accumulator >= fixed_dt) {
                if (on_update) on_update();
                update_count++;
                accumulator -= fixed_dt;
            }

            draw_count++;
            if (on_draw) on_draw();

            // Автоматический FPS
            if (show_fps && font) {
                char buf[32];
                sprintf(buf, "FPS: %.1f", fps);
                //draw_text(buf, 30, 30);
            }

            SDL_GL_SwapWindow(window);

        }
    }

    void update_delta_and_fps() {
        Uint32 now = SDL_GetTicks();

        fps_frames++;
        if (now - fps_start_time >= 1000) {
            fps = fps_frames * 1000.0f / (now - fps_start_time);
            fps_frames = 0;
            fps_start_time = now;
        }

        dt = (now - last_frame_time) / 1000.0f;
        last_frame_time = now;
    }

    void dbg_print_frames() {
        static Uint32 log_timer = 0;
        Uint32 now = SDL_GetTicks();
        if (now - log_timer >= 1000) {
            SDL_Log("PHYS updates/sec: %llu", update_count);
            SDL_Log("RENDER frames/sec: %llu  (shown FPS: %.1f)", draw_count, fps);
            SDL_Log("Ratio draw/update: %.1f", (float)draw_count / update_count);
            update_count = 0;
            draw_count = 0;
            log_timer = now;
        }
    }

};

GLuint load_texture_opengl(const char* path) {
    SDL_Surface* surf = IMG_Load(path);
    if (!surf) return 0;

    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    // Настройка фильтрации (чтобы не было мыла в 2D)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Загружаем данные из SDL_Surface в OpenGL
    GLint format = (surf->format->BytesPerPixel == 4) ? GL_RGBA : GL_RGB;
    // Если цвета инвертированы (красный и синий), используй GL_BGRA
    if (surf->format->BytesPerPixel == 4 && surf->format->Rmask == 0x000000ff) {
        // оставляем GL_RGBA
    } else if (surf->format->BytesPerPixel == 4) {
        format = GL_BGRA;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, format, surf->w, surf->h, 0, format, GL_UNSIGNED_BYTE, surf->pixels);

    SDL_FreeSurface(surf);
    return texID;
}

} // namespace lvichki
