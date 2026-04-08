#pragma once
#include <vector>
#include <algorithm>
#include "lib/v2.h"
#include "mat4.h"

struct clr {
    float r, g, b, a;
};

class Transform {
public:
    v2 position = {0, 0};
    float rotation = 0;
    v2 scale = {1, 1};
    v2 sprite_size = {128, 128};

    GLuint texture;
    clr color = {1.0f, 1.0f, 1.0f, 1.0f};

    Transform* parent = nullptr;
    std::vector<Transform*> children;

    mat4 get_local_matrix() {
        return mat4::translation(position.x, position.y) *
               mat4::rotationZ(rotation) *
               mat4::scale(scale.x, scale.y);
    }

    mat4 get_world_matrix() {
        if (parent) {
            // Матрица родителя * локальная матрица
            return parent->get_world_matrix() * get_local_matrix();
        }
        return get_local_matrix();
    }

    void set_parent(Transform* new_parent) {
        // Удаляемся из старого родителя
        if (parent) {
            auto& c = parent->children;
            c.erase(std::remove(c.begin(), c.end(), this), c.end());
        }
        parent = new_parent;
        if (parent) {
            parent->children.push_back(this);
        }
    }

    void draw(GLuint shaderProgram, const mat4& projection) {
        GLint loc = glGetUniformLocation(shaderProgram, "u_mvp");
        GLint colLoc = glGetUniformLocation(shaderProgram, "u_color");
        GLint texLoc = glGetUniformLocation(shaderProgram, "u_texture");

        // 1. Считаем матрицу рендеринга
        // Берем мировую матрицу из твоей системы (которая наследует трансформации)
        mat4 mWorld = get_world_matrix(); 
        
        // Накладываем scale спрайта (чтобы не ломать иерархию детей через scale родителя)
        mat4 mRender = mWorld * mat4::scale(sprite_size.x, sprite_size.y, 1.0f);
        mat4 mvp = projection * mRender;

        // 2. Передаем данные в шейдер
        glUniformMatrix4fv(loc, 1, GL_FALSE, mvp.m);
        glUniform4f(colLoc, color.r, color.g, color.b, color.a);

        // 3. Отрисовка
        glBindVertexArray(lvichki::quadVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(texLoc, 0);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        // 4. Рекурсия (если есть дочерние объекты)
        for (auto* child : children) {
            child->draw(shaderProgram, projection);
        }
    }

    void dump(int depth = 0) {
        string indent(depth * 4, ' ');
        mat4 world = get_world_matrix();
        
        SDL_Log("%s--- Node: %p ---", indent.c_str(), this);
        SDL_Log("%sLocal Pos: (%.2f, %.2f)", indent.c_str(), position.x, position.y);
        
        // Вывод матрицы 4х4 квадратом
        SDL_Log("%sWorld Matrix:", indent.c_str());
        for (int i = 0; i < 4; i++) {
            // Чтобы получить строку i, нам нужно взять i-й элемент из каждого столбца
            SDL_Log("%s  [ %7.2f %7.2f %7.2f %7.2f ]", 
                    indent.c_str(),
                    world.m[0 + i],   // Столбец 0, строка i
                    world.m[4 + i],   // Столбец 1, строка i
                    world.m[8 + i],   // Столбец 2, строка i
                    world.m[12 + i]); // Столбец 3, строка i (ТУТ ДОЛЖНА БЫТЬ ПОЗИЦИЯ)
        }

        // Рекурсия для детей
        for (auto child : children) {
            child->dump(depth + 1);
        }
    }

};