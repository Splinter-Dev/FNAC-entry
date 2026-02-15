#include "editor/editor.hh"
#include <raylib.h>
#include <raymath.h>

const float STEP = 1.0f / 500.0f;  // fixed timestep: 500 updates per second
                                   // independent of the framerate

#define HEADER_HEIGHT 100


Editor editor;

#include "entityFactory.hh"

int main (int argc, char *argv[]) {
    RegisterAllEntities();

    // Antialiasing
    SetConfigFlags(FLAG_MSAA_4X_HINT);

    // Vsync
    SetConfigFlags(FLAG_VSYNC_HINT);

    InitWindow(1920, 1080, "Map Editor");

    // Exit key
    SetExitKey(KEY_ESCAPE);

    editor.init();

    float accumulator = 0.0f;

    while (!WindowShouldClose() && editor.running) {
        float dt = GetFrameTime();

        accumulator += dt;

        while (accumulator >= STEP) {
            updateWorld(editor.world, STEP);
            accumulator -= STEP;
        }

        BeginDrawing();
        editor.render();
        EndDrawing();
    }

    return 0;
}
