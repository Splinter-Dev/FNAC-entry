#include "scene.hh"
#include <cmath>

void Scene::init() {

    physicalWidth = GetScreenWidth();
    physicalHeight = GetScreenHeight();
    target = LoadRenderTexture(logicalWidth, logicalHeight);

    camera.projection = CAMERA_ORTHOGRAPHIC;
    camera.position   = { 0.0f, 20.0f, 20.0f };
    camera.target     = { 0.0f, 0.0f, 0.0f };
    camera.up         = { 0.0f, 1.0f, 0.0f };
    camera.fovy       = 25.0f;
}

void Scene::unload() {
    UnloadRenderTexture(target);
}

void Scene::renderToTexture(const std::function<void()>& drawWorld) {
    BeginTextureMode(target);
        ClearBackground(BLACK);
        BeginMode3D(camera);
            drawWorld(); // your world drawing goes here
        EndMode3D();
    EndTextureMode();
}

void Scene::draw(Rectangle viewport) {

    // Scale based on viewport, not physical screen
    float scaleX = viewport.width  / logicalWidth;
    float scaleY = viewport.height / logicalHeight;
    float scale = (scaleX < scaleY) ? scaleX : scaleY;

    DrawTextureInViewport(viewport, scale, scale);
}

void Scene::drawScaled(Rectangle viewport) {

    // Scale based on viewport, not physical screen
    float scaleX = viewport.width  / logicalWidth;
    float scaleY = viewport.height / logicalHeight;

    DrawTextureInViewport(viewport, scaleX, scaleY);
}

void Scene::DrawTextureInViewport(Rectangle viewport, float scaleX, float scaleY) {

    float scaledWidth  = logicalWidth  * scaleX;
    float scaledHeight = logicalHeight * scaleY;

    // Center inside the viewport (letterboxing)
    Rectangle dest = {
        viewport.x + (viewport.width  - scaledWidth)  * 0.5f,
        viewport.y + (viewport.height - scaledHeight) * 0.5f,
        scaledWidth,
        scaledHeight
    };

    Rectangle sourceRec = {
        0.0f,
        0.0f,
        (float)target.texture.width,
        -(float)target.texture.height  // keep flipped for RenderTexture
    };

    Vector2 origin = { 0.0f, 0.0f };

    DrawTexturePro(target.texture, sourceRec, dest, origin, 0.0f, WHITE);
}

void Scene::drawFullscreen() {
    float scaleX = (float)physicalWidth / logicalWidth;
    float scaleY = (float)physicalHeight / logicalHeight;
    float scale = fminf(scaleX, scaleY);
    Rectangle destRec = { 
        0, 0, 
        logicalWidth * scale, 
        logicalHeight * scale};
    draw(destRec);
}

void Scene::update() {
    physicalWidth = GetScreenWidth();
    physicalHeight = GetScreenHeight();
}
