#pragma once

#include <functional>
#include "raylib.h"

struct Scene {
    const int logicalWidth = 1920;
    const int logicalHeight = 1080;

    int physicalWidth = 1920;
    int physicalHeight = 1080;

    Camera3D camera = {0};
    RenderTexture2D target;

    // Initialize the scene, this is needed to get the
    // physical resolution of the screen
    void init();

    // Update sceen width / height and camera projection
    void update();

    // Render the scene into the render texture using a user-supplied lambda
    void renderToTexture(const std::function<void()>& drawWorld);

    // Draw the render texture scaled to the physical screen (or any rectangle)
    void draw(Rectangle viewport);

    // Draw the render texture scaled to the physical screen (or any rectangle)
    void drawScaled(Rectangle viewport);

    // Convenience: scale to fullscreen with aspect ratio
    void drawFullscreen();

    void unload();

    private:

    void DrawTextureInViewport(Rectangle viewport, float scaleX, float scaleY);
};
