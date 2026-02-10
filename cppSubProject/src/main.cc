#include "raylib.h"
#include "raymath.h"
#include "player.hh"
#include "shader.hh"
#include "world.hh"
#include "rlgl.h"
#include "scene.hh"

const float STEP = 1.0f / 500.0f;  // fixed timestep: 500 updates per second
                                   // independent of the framerate

World mainWorld;

#define HELP "Press SPACE to toggle shadow mode\n" \
             "Press W/S to move forward/backward\n" \
             "Press A/D to move left/right\n" \
             "Press SHIFT to increase velocity\n" \
             "Press ESC to exit"

void updateGame(World & world, float delta) {
    updateWorld(world, delta);
}

int main(void)
{
    // Antialiasing
    SetConfigFlags(FLAG_MSAA_4X_HINT);

    // Vsync
    SetConfigFlags(FLAG_VSYNC_HINT);

    InitWindow(1920, 1080, "Top-Down Cube Example");

    // Audio test
    InitAudioDevice();
    Music m = LoadMusicStream("resources/menuideia.wav");

    SetMasterVolume(0.5f);
    SetMusicVolume(m, 0.5f);
    SetMusicPitch(m, 0.9f);

    // Exit key
    SetExitKey(KEY_ESCAPE);

    // Initialize the toon shader
    initShaders();

    Scene scene;

    scene.Init(); // Logical resolution is 1920x1080

    scene.camera.projection = CAMERA_ORTHOGRAPHIC;
    scene.camera.position = { 0.0f, 20.0f, 20.0f }; // farther back along diagonal
    scene.camera.target   = { 0.0f, 0.0f, 0.0f };
    scene.camera.up       = { 0.0f, 1.0f, 0.0f };   // keep Y as "up" on screen
    scene.camera.fovy     = 25.0f; // can be slightly smaller now

    Model playerModel = LoadModel("resources/guy.iqm");
    Texture2D playerTexture = LoadTexture("resources/guytex.png");

    Player * player = new Player(
                &playerModel,
                &playerTexture,
                0.5f);

    addEntity(mainWorld, std::unique_ptr<Entity>(player));


    player->model->materials[0].shader = toonShader;
    player->setPosition((Vector3){ 0.0f, 0.1f, 0.0f });

    Light light = pointLight((Vector3){ 0.0f, 10.0f, 0.0f }, RED, 1.0f, 10.0f);
    Light light2 = pointLight((Vector3){ 10.0f, 10.0f, 20.0f }, BLUE, 1.0f, 8.0f);

    addLight(mainWorld, std::move(light));
    addLight(mainWorld, std::move(light2));

    float accumulator = 0.0f;

    PlayMusicStream(m);

    while (!WindowShouldClose())
    {
        UpdateMusicStream(m);

        float dt = GetFrameTime();

        accumulator += dt;

        // Process fixed-size updates
        while (accumulator >= STEP) {
            updateGame(mainWorld, STEP);
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
        if (IsKeyPressed(KEY_SPACE)) {
            player->toggleShadowMode();
        }
        
        if (IsKeyDown(KEY_LEFT_SHIFT)) {
            player->setVelocity(20.0f);
        } else {
            player->setVelocity(10.0f);
        }

        // Update the shader with the camera view vector (points towards { 0.0f, 0.0f, 0.0f })
        float cameraPos[3] = { 
            scene.camera.position.x, 
            scene.camera.position.y, 
            scene.camera.position.z };
        SetShaderValue(toonShader, toonShader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);

        scene.camera.target = player->getPosition();
        scene.camera.position =
            (Vector3){
                player->getPosition().x,
                player->getPosition().y + 20.0f,
                player->getPosition().z + 20.0f
            };

        BeginDrawing();

            ClearBackground(DARKGRAY);

            /* Everything inside this lambda will be rendered to a texture,
             * then loaded to the physical screen */
            scene.RenderToTexture([&]() {
                    BeginShaderMode(toonShader);
                        DrawPlane(Vector3{ 0.0f, 0.0f, 0.0f }, Vector2{ 80.0f, 80.0f }, DARKPURPLE);
                    EndShaderMode();
                    renderWorld(mainWorld, scene.camera);
                    // renderBoundingBoxes(mainWorld);
                });

            scene.DrawFullscreen();

            DrawText(HELP, 10, 10, 20, 
                    (Color){ 255, 255, 255, 150 });

            DrawFPS(1830, 10);
        EndDrawing();
    }

    UnloadShader(toonShader);
    UnloadModel(playerModel);
    UnloadTexture(playerTexture);

    StopMusicStream(m);
    UnloadMusicStream(m);

    CloseWindow();

    return 0;
}
