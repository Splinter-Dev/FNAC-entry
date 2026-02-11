#include "scene.hh"
#include <cmath>

void Scene::Init() {
    physicalWidth = GetScreenWidth();
    physicalHeight = GetScreenHeight();
    target = LoadRenderTexture(logicalWidth, logicalHeight);

    camera.position = { 0.0f, 10.0f, 10.0f };
    camera.target   = { 0.0f, 0.0f, 0.0f };
    camera.up       = { 0.0f, 1.0f, 0.0f };
    camera.fovy     = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
}

void Scene::Unload() {
    UnloadRenderTexture(target);
}

void Scene::RenderToTexture(const std::function<void()>& drawWorld) {
    BeginTextureMode(target);
        ClearBackground(BLACK);
        BeginMode3D(camera);
            drawWorld(); // your world drawing goes here
        EndMode3D();
    EndTextureMode();
}

void Scene::Draw(Rectangle viewport) {
    float scaleX = (float)physicalWidth / logicalWidth;
    float scaleY = (float)physicalHeight / logicalHeight;

    float scale = (scaleX < scaleY) ? scaleX : scaleY;

    Rectangle dest = {
        viewport.x,
        viewport.y,
        logicalWidth * scale,
        logicalHeight * scale
    };

    Rectangle sourceRec = {
        0.0f,
        0.0f,
        (float)target.texture.width,
        -(float)target.texture.height
    };

    Vector2 origin = { 0.0f, 0.0f };

    DrawTexturePro(target.texture, sourceRec, dest, origin, 0.0f, WHITE);
}

void Scene::DrawFullscreen() {
    float scaleX = (float)physicalWidth / logicalWidth;
    float scaleY = (float)physicalHeight / logicalHeight;
    float scale = fminf(scaleX, scaleY);

    Rectangle destRec = { 0, 0, logicalWidth * scale, logicalHeight * scale };
    Draw(destRec);
}

void Scene::Update() {
    physicalWidth = GetScreenWidth();
    physicalHeight = GetScreenHeight();
}
