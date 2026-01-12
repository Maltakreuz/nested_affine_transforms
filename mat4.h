#pragma once

struct mat4 {
    float m[16]; // Данные хранятся столбцами

    mat4() { identity(); }

    void identity() {
        for(int i=0; i<16; i++) m[i] = 0;
        m[0] = 1.0f; m[5] = 1.0f; m[10] = 1.0f; m[15] = 1.0f;
    }

    // Умножение матриц (A * B)
    mat4 operator*(const mat4& b) const {
        mat4 res;
        for (int row = 0; row < 4; ++row) {
            for (int col = 0; col < 4; ++col) {
                res.m[col * 4 + row] = 
                    m[0 * 4 + row] * b.m[col * 4 + 0] +
                    m[1 * 4 + row] * b.m[col * 4 + 1] +
                    m[2 * 4 + row] * b.m[col * 4 + 2] +
                    m[3 * 4 + row] * b.m[col * 4 + 3];
            }
        }
        return res;
    }

    static mat4 translation(float x, float y) {
        mat4 res; res.identity();
        res.m[12] = x; res.m[13] = y; // Последний столбец
        return res;
    }

    static mat4 rotationZ(float angle_rad) {
        mat4 res; res.identity();
        float s = sin(angle_rad);
        float c = cos(angle_rad);
        res.m[0] = c;  res.m[1] = s;
        res.m[4] = -s; res.m[5] = c;
        return res;
    }

    static mat4 scale(float sx, float sy) {
        mat4 res; res.identity();
        res.m[0] = sx; res.m[5] = sy;
        return res;
    }

    // Ортографическая проекция для 2D (экран в пикселях)
    static mat4 ortho(float left, float right, float bottom, float top) {
        mat4 res; res.identity();
        res.m[0] = 2.0f / (right - left);
        res.m[5] = 2.0f / (top - bottom);
        res.m[10] = -1.0f; 
        res.m[12] = -(right + left) / (right - left);
        res.m[13] = -(top + bottom) / (top - bottom);
        res.m[15] = 1.0f;
        return res;
    }
};
