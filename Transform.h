#pragma once
#include <vector>
#include <algorithm>
#include "lib/v2.h"
#include "mat4.h"

class Transform {
public:
    v2 position = {0, 0};
    float rotation = 0;
    v2 scale = {1, 1};

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
};