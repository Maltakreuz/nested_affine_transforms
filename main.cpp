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


Transform* create_transform(const char* texture_path, const v2& pos, const v2& scale = {1.0f, 1.0f}, float rotation = 0.0f) {
    Transform* t = new Transform();
    t->position = pos;
    t->scale = scale;
    t->texture = load_texture_opengl(texture_path);
    t->rotation = rotation;
    return t;
}

Transform* create_transforms() {
    Transform* parent = create_transform("lionessy_face.png", {960, 540}, {1.8f, 1.8f});
    Transform* child = create_transform("lionessy_face.png", {200, 0}, {1.5f, 1.5f});
    child->set_parent(parent);
    return parent;
}

int main(int, char**) {
    Game game;
    print_all_gpu_info(); // Раскомментируй, если нужно видеть логи GPU
    mat4 projection = mat4::ortho(0, 1920, 1080, 0); 

    Transform* root = create_transforms();

    game.on_update = [&]() {
        root->rotation += 0.5f * game.fixed_dt;
    };

    game.on_draw = [&]() {
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(game.shaderProgram);
        root->draw(game.shaderProgram, projection);
    };

    SDL_Log("Press 'P' to dump the scene graph to the console.");

    SDL_Log("--- SCENE GRAPH DUMP ---");
    root->dump();
    SDL_Log("------------------------\n\n");

    game.on_event = [&](const SDL_Event& e) {
        if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_p) {
                SDL_Log("--- SCENE GRAPH DUMP ---");
                root->dump();
                SDL_Log("------------------------");
            }
        }
    };

    game.run();

    return 0;
}