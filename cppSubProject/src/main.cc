#include "raylib.h"
#include "raymath.h"
#include "bird.hh"
#include "player.hh"

const float STEP = 1.0f / 500.0f;  // fixed timestep: 500 updates per second
                                   // independent of the framerate
                                   //
World mainWorld = newWorld();

void updateGame(World & world, float delta) {
    updateWorld(world, delta);
}

int main(void)
{
    mainWorld.backgroundColor = DARKGRAY;

    // Antialiasing
    SetConfigFlags(FLAG_MSAA_4X_HINT);

    // Vsync
    InitWindow(1920, 1080, "Top-Down Cube Example");

    // Exit key
    SetExitKey(KEY_ESCAPE);

    // Set up a camera directly above the origin, looking down
    Camera3D camera = { 0 };
    camera.position = (Vector3){ 0.0f, 20.0f, 10.0f };   // Above the scene
    camera.target   = (Vector3){ 0.0f, 0.0f, 0.0f };    // Looking at the origin
    camera.up       = (Vector3){ 0.0f, 0.0f, -1.0f };   // Rotate so "up" is screen up
    camera.fovy     = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    Player * player = std::make_unique<Player>().release();
    player->setVelocity(10.0f);

    Bird * bird = std::make_unique<Bird>(
            "resources/bird.obj",
            0.2f,
            10.0f,
            -1.0f
            ).release();

    addEntity(mainWorld, std::unique_ptr<Entity>(player));
    addEntity(mainWorld, std::unique_ptr<Entity>(bird));

    float accumulator = 0.0f;
    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        accumulator += dt;

        // Process fixed-size updates
        while (accumulator >= STEP) {
            updateGame(mainWorld, STEP);
            accumulator -= STEP;
        }

        player->setDirection(Vector3Zero());

        if (IsKeyDown(KEY_A)) {
            player->setDirectionX(-1.0f);
        } 
        if (IsKeyDown(KEY_D)) {
            player->setDirectionX(1.0f);
        } 
        if (IsKeyDown(KEY_W)) {
            player->setDirectionZ(-1.0f);
        } 
        if (IsKeyDown(KEY_S)) {
            player->setDirectionZ(1.0f);
        }
        player->normalizeDirection();

        // Move camera
        if (IsKeyDown(KEY_LEFT)) {
            camera.position.x -= 0.2f;
        } else if (IsKeyDown(KEY_RIGHT)) {
            camera.position.x += 0.2f;
        }
        if (IsKeyDown(KEY_UP)) {
            camera.position.y += 0.2f;
        } else if (IsKeyDown(KEY_DOWN)) {
            if (camera.position.y > 0.2f) {
                camera.position.y -= 0.2f;
            }
        }

        // Enable shadow mode
        if (IsKeyPressed(KEY_SPACE)) {
            player->toggleShadowMode();
        }
        
        if (IsKeyDown(KEY_LEFT_SHIFT)) {
            player->setVelocity(20.0f);
        } else {
            player->setVelocity(10.0f);
        }

        BeginDrawing();

        BeginMode3D(camera);

        renderWorld(mainWorld, camera);
        // renderBoundingBoxes(mainWorld);

        DrawGrid(100, 1.0f);

        EndMode3D();
        DrawFPS(10, 10);
        EndDrawing();
    }

    clearEntities(mainWorld);

    CloseWindow();

    return 0;
}

