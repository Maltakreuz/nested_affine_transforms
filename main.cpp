#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "lib/Game.h"
#include "lib/Color.h"
#include "lib/v2.h"
#include "mat4.h"
#include "Transform.h"

using namespace lvichki;
using namespace std;

int main(int, char**) {
    Game game;

    auto texture = load_texture_opengl("lizard_archer_template.png");
    cout << "texture loaded: " << texture << endl;

    mat4 projection = mat4::ortho(0, 1920, 1080, 0); // 0,0 - левый верхний угол

    Transform parent;
    parent.position = {960, 540}; // Центр экрана

    Transform child;
    child.set_parent(&parent);
    child.position = {200, 0}; // Относительно родителя!
    child.scale = {100, 100};  // Размер спрайта

    game.on_update = [&]() {
        parent.rotation += 1.0f * game.fixed_dt; // Вращаем родителя
        child.rotation += 2.0f * game.fixed_dt;  // Ребенок крутится вокруг своей оси И родителя
    };

    game.on_draw = [&]() {
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f); // Сделаем фон поспокойнее
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(game.shaderProgram);
        
        // 1. Активируем текстурный юнит и привязываем текстуру
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        
        // Сообщаем шейдеру, что u_texture — это 0-й юнит
        GLint texLoc = glGetUniformLocation(game.shaderProgram, "u_texture");
        glUniform1i(texLoc, 0);

        // 2. Устанавливаем белый цвет (чтобы текстура была без тинта)
        GLint colLoc = glGetUniformLocation(game.shaderProgram, "u_color");
        glUniform4f(colLoc, 1.0f, 1.0f, 1.0f, 1.0f); 

        // 3. Матрицы и отрисовка
        glBindVertexArray(game.quadVAO);  
        mat4 model = child.get_world_matrix();
        mat4 mvp = projection * model;
        
        GLint loc = glGetUniformLocation(game.shaderProgram, "u_mvp");
        glUniformMatrix4fv(loc, 1, GL_FALSE, mvp.m);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
    };
    
    game.on_event = [&](const SDL_Event& e) {
        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_F5) {
            dbg_draw = !dbg_draw;
        }
    };

    game.run();
    return 0;
}
