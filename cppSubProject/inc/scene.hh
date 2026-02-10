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

    void Init();

    void Unload();

    // Render the scene into the render texture using a user-supplied lambda
    void RenderToTexture(const std::function<void()>& drawWorld);

    // Draw the render texture scaled to the physical screen (or any rectangle)
    void Draw(Rectangle viewport);

    // Convenience: scale to fullscreen with aspect ratio
    void DrawFullscreen();
};
