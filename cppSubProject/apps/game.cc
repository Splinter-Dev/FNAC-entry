#include "light.hh"
#include "raylib.h"
#include "raymath.h"
#include "entities/player.hh"
#include "entities/cube.hh"
#include "world.hh"
#include "rlgl.h"
#include "scene.hh"
#include "shaderManager.hh"
#include "resourceManager.hh"

const float STEP = 1.0f / 500.0f;  // fixed timestep: 500 updates per second
                                   // independent of the framerate

World mainWorld;

#define HELP "Press SPACE to toggle shadow mode\n" \
             "Press W/S to move forward/backward\n" \
             "Press A/D to move left/right\n" \
             "Press SHIFT to increase velocity\n" \
             "Press ESC to exit"

int main(void)
{
    // Antialiasing
    SetConfigFlags(FLAG_MSAA_4X_HINT);

    // Vsync
    SetConfigFlags(FLAG_VSYNC_HINT);

    InitWindow(1920, 1080, "Top-Down Cube Example");

    // Audio test
    InitAudioDevice();

    // Exit key
    SetExitKey(KEY_ESCAPE);

    Scene scene;
    scene.init();   // Logical resolution is 1920x1080

    Model * playerModel = RM.getModel("resources/guy.iqm");
    Texture2D * playerTexture = RM.getTexture("resources/guytex.png");

    Player * player = new Player(
                playerModel,
                playerTexture,
                0.5f);


    Cube * cube = new Cube(
                (Vector3){ 10.0f, 2.5f, 0.0f },
                2.5f,
                2.5f);

    player->setGravity(30.0f);
    player->setJumpHeight(5.0f);
    player->setPosition((Vector3){ 0.0f, 5.0f, 0.0f });

    addEntityToWorld(mainWorld, std::unique_ptr<Entity>(player));
    addEntityToWorld(mainWorld, std::unique_ptr<Entity>(cube));

    Light light = pointLight((Vector3) {0.0f, 10.0f, 0.0f}, RED, 1.0f, 10.0f);
    Light light2 = pointLight((Vector3) {10.0f, 10.0f, 20.0f}, WHITE, 1.0f, 10.0f);

    addLightToWorld(mainWorld, std::move(light));
    addLightToWorld(mainWorld, std::move(light2));

    float accumulator = 0.0f;

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        accumulator += dt;

        // Process fixed-size updates
        while (accumulator >= STEP) {
            updateWorld(mainWorld, STEP);
            accumulator -= STEP;
        }

        Vector3 dir = Vector3Zero();

        if (IsKeyDown(KEY_A)) dir.x = -1.0f;
        if (IsKeyDown(KEY_D)) dir.x = 1.0f;
        if (IsKeyDown(KEY_W)) dir.z = -1.0f;
        if (IsKeyDown(KEY_S)) dir.z = 1.0f;

        dir = Vector3Normalize(dir);

        player->setDirection(dir);

        // Enable shadow mode
        if (IsKeyPressed(KEY_J)) {
            player->toggleShadowMode();
        }
        
        if (IsKeyDown(KEY_LEFT_SHIFT)) {
            player->setVelocity(20.0f);
        } else {
            player->setVelocity(10.0f);
        }

        if (IsKeyDown(KEY_SPACE)) {
            player->jump();
        }

        // Follow the player
        scene.camera.target = player->getPosition();
        scene.camera.position =
            (Vector3){
                player->getPosition().x,
                player->getPosition().y + 20.0f,
                player->getPosition().z + 20.0f
            };

        // Update physical with / height
        scene.update();

        BeginDrawing();

            ClearBackground(DARKGRAY);

            /* Everything inside this lambda will be rendered to a texture,
             * then loaded to the physical screen */
            scene.renderToTexture([&]() {
                    BeginShaderMode(SM.getToon());
                        DrawPlane(Vector3{ 0.0f, 0.0f, 0.0f }, Vector2{ 80.0f, 80.0f }, DARKPURPLE);
                    EndShaderMode();
                    renderWorld(mainWorld, scene.camera);
                    // renderBoundingBoxes(mainWorld);
                    renderHitBoxes(mainWorld);
                });

            scene.drawFullscreen(); // Draw the texture to the screen

            DrawText(HELP, 10, 10, 20, 
                    (Color){ 255, 255, 255, 150 });

            DrawFPS(1830, 10);

        EndDrawing();
    }

    SM.unloadAll();

    CloseWindow();

    return 0;
}
