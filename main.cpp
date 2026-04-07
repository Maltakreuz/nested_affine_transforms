#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "lib/Game.h"
#include "lib/Color.h"
#include "lib/v2.h"
#include "mat4.h"
#include "Transform.h"
#include "gpu_info.h"

using namespace lvichki;
using namespace std;

int main(int, char**) {
    Game game;
    print_all_gpu_info(); // Раскомментируй, если нужно видеть логи GPU

    // 1. Загрузка ресурсов
    auto texture = load_texture_opengl("lionessy_face.png");
    mat4 projection = mat4::ortho(0, 1920, 1080, 0); 

    // 2. Настройка объектов
    Transform parent;
    parent.position = {960, 540}; // Центр экрана
    parent.scale = {1, 1}; // Сделаем родителя чуть побольше

    Transform child;
    child.set_parent(&parent);
    child.position = {200, 0};
    child.scale = {1, 1};

    // 3. Создаем буферы для линии (динамические)
    GLuint lineVAO, lineVBO;
    glGenVertexArrays(1, &lineVAO);
    glGenBuffers(1, &lineVBO);
    glBindVertexArray(lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0); // Используем location = 0 (aPos в шейдере)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    game.on_update = [&]() {
        parent.rotation += 0.5f * game.fixed_dt; // Медленно вращаем всю систему
        child.rotation += 2.0f * game.fixed_dt;  // Ребенок крутится сам
    };

    game.on_draw = [&]() {
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(game.shaderProgram);
        GLint loc = glGetUniformLocation(game.shaderProgram, "u_mvp");
        GLint colLoc = glGetUniformLocation(game.shaderProgram, "u_color");
        GLint texLoc = glGetUniformLocation(game.shaderProgram, "u_texture");

        // --- ШАГ 1: РИСУЕМ РОДИТЕЛЯ ---
        glBindVertexArray(game.quadVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(texLoc, 0);

        // Получаем чистую мировую матрицу (там scale 1,1)
        mat4 mParent = parent.get_world_matrix(); 
        
        // Накладываем размер спрайта ТОЛЬКО для отрисовки
        mat4 mParentRender = mParent * mat4::scale(parent.sprite_size.x, parent.sprite_size.y, 1.0f);
        
        mat4 mvpParent = projection * mParentRender;
        glUniformMatrix4fv(loc, 1, GL_FALSE, mvpParent.m);
        glUniform4f(colLoc, 0.4f, 0.6f, 1.0f, 1.0f); 
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // --- ШАГ 2: РИСУЕМ РЕБЕНКА ---
        mat4 mChild = child.get_world_matrix(); 
        
        // Ребенок наследует всё от родителя, но размер у него свой
        mat4 mChildRender = mChild * mat4::scale(child.sprite_size.x, child.sprite_size.y, 1.0f);
        
        mat4 mvpChild = projection * mChildRender;
        glUniformMatrix4fv(loc, 1, GL_FALSE, mvpChild.m);
        glUniform4f(colLoc, 1.0f, 1.0f, 1.0f, 1.0f); 
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // --- ШАГ 3: РИСУЕМ ЛИНИЮ СВЯЗИ ---
        // Линию рисуем между ЦЕНТРАМИ ТРАНСФОРМОВ (им плевать на размер спрайта)
        float lineData[] = {
            mParent.m[12], mParent.m[13], 
            mChild.m[12],  mChild.m[13]
        };

        glBindVertexArray(lineVAO);
        glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(lineData), lineData);

        glUniformMatrix4fv(loc, 1, GL_FALSE, projection.m);
        glUniform4f(colLoc, 1.0f, 0.2f, 0.2f, 1.0f); 
        glBindTexture(GL_TEXTURE_2D, 0); 
        
        glLineWidth(2.0f); 
        glDrawArrays(GL_LINES, 0, 2);
    };

    game.run();

    // Очистка
    glDeleteVertexArrays(1, &lineVAO);
    glDeleteBuffers(1, &lineVBO);

    return 0;
}