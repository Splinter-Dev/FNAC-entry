#include "editor/editor.hh"
#include "raylib.h"
#include "world.hh"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define HEADER_HEIGHT 50
#define HEADER_BUTTON_SPACING 10

#define NEW_ENTITY_BUTTON_REC (Rectangle){ 5.0f, 5.0f, 150.0f, 40.0f }

#define CROSSHAIR_SIZE 10

void Editor::init() {
    scene.init();

    GuiSetStyle(DEFAULT, TEXT_SIZE, 20);

    headerButtons.push_back({
        "Entities",
        [&]() {
            popupWarning("Not implemented");
        }
    });

    headerButtons.push_back({
        "Lights",
        [&]() {
            popupWarning("Not implemented");
        }
    });

    headerButtons.push_back({
        "World",
        [&]() {
            popupWarning("Not implemented");
        }
    });

    headerButtons.push_back({ 
        "Edit",
        [&]() {
            popupWarning("Not implemented");
        }
    });

    headerButtons.push_back({
            scene.camera.projection == CAMERA_PERSPECTIVE ? 
            "Perspective" : "Orthographic",
        [&]() {
            popupWarning("Not implemented");
        }
    });

    headerButtons.push_back({
        "Exit",
        [&]() {
            running = false;
        }
    });
}

void Editor::handleKeys() {

    static Vector3 lastScenePosition = { 0.0f, 0.0f, 0.0f };
    static Vector3 lastSceneTarget = { 0.0f, 0.0f, 0.0f };

    Vector2 mouse = GetMousePosition();

    // Start drag
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        lastScenePosition = scene.camera.position;
        lastSceneTarget = scene.camera.target;
        dragStart = mouse;
        dragDelta = {0, 0};
        dragging = true;
    }

    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
    {
        dragStart = mouse;
        dragDelta = {0, 0};
        selecting = true;
    }

    // Update drag while held
    if (dragging && IsMouseButtonDown(MOUSE_LEFT_BUTTON))
    {
        float panSpeed = 0.025f * (scene.camera.fovy / 25.0f); // tune this
        dragDelta.x = mouse.x - dragStart.x;
        dragDelta.y = mouse.y - dragStart.y;
        scene.camera.position.x = lastScenePosition.x - dragDelta.x * panSpeed;
        scene.camera.position.z = lastScenePosition.z - dragDelta.y * panSpeed;
        scene.camera.target.x = lastSceneTarget.x - dragDelta.x * panSpeed;
        scene.camera.target.z = lastSceneTarget.z - dragDelta.y * panSpeed;
    }

    if (selecting && IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
    {
        selectionRect.x = std::min(mouse.x, dragStart.x);
        selectionRect.y = std::min(mouse.y, dragStart.y);
        selectionRect.width = std::abs(mouse.x - dragStart.x);
        selectionRect.height = std::abs(mouse.y - dragStart.y);
    }

    // End drag
    if (dragging && IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
    { dragging = false; }
    if (selecting && IsMouseButtonReleased(MOUSE_RIGHT_BUTTON))
    { selecting = false; }

    float scroll = GetMouseWheelMove();
    // Zoom
    if (scroll != 0.0f) {
        scene.camera.fovy -= scroll * 0.5f;
        scene.camera.fovy = std::max(scene.camera.fovy, 1.0f);
    }
}

void Editor::popupWarning(const std::string & message) {
    if (popup) { return; }
    popupWarningMessage = message;
    popup = true;
}

void Editor::render() {

    scene.renderToTexture([&]() {
                ClearBackground(DARKGRAY);

                // Draw the grid
                if (showGrid) { DrawGrid(100, 1.0f); }

                renderWorld(world, scene.camera);
                if (renderHitBoxes) renderWorld(world, scene.camera);
                if (renderAABBs) renderBoundingBoxes(world);
                });

    scene.drawFullscreen();

    // Render crosshair
    DrawLine(GetScreenWidth() / 2, GetScreenHeight() / 2 - CROSSHAIR_SIZE, 
            GetScreenWidth() / 2, GetScreenHeight() / 2 + CROSSHAIR_SIZE,
            RED);
    DrawLine(GetScreenWidth() / 2 - CROSSHAIR_SIZE, GetScreenHeight() / 2, 
            GetScreenWidth() / 2 + CROSSHAIR_SIZE, GetScreenHeight() / 2,
            RED);

    Vector2 mouse = GetMousePosition();

    Rectangle headerRec = {
        0.0f,
        0.0f,
        (float)GetScreenWidth(),
        HEADER_HEIGHT
    };

    if (!CheckCollisionPointRec(mouse, headerRec)) {
        handleKeys();
    }

    DrawRectangleRec(headerRec, 
            GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

    // Draw the header
    DrawRectangle(0, 0, GetScreenWidth(), HEADER_HEIGHT, 
            GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

    int count = (int)headerButtons.size();
    float spacing = HEADER_BUTTON_SPACING;
    float screenW = (float)GetScreenWidth();

    float buttonWidth = (screenW - spacing * (count + 1)) / count;

    Rectangle buttonRec = {
        spacing,
        spacing,
        buttonWidth,
        35
    };


    for (auto& button : headerButtons)
    {
        if (GuiButton(buttonRec, button.text.c_str()))
            button.callback();

        buttonRec.x += buttonWidth + spacing;
    }

    // Draw selection
    if (selecting) {
        DrawRectangleLinesEx(selectionRect, 1, GREEN);
    }

    // Draw the popup
    if (popup) {
        Rectangle popupRec = {
            GetScreenWidth() / 2.0f - 400.0f / 2.0f,
            GetScreenHeight() / 2.0f - 100.0f / 2.0f,
            400.0f,
            100.0f
        };

        int result = GuiMessageBox(popupRec, "Message Box", 
                popupWarningMessage.c_str(), "Yes;Of course");

        if (result == 1 || result == 2 || result == 0) {
            popup = false;
        }
    }

    // Show screen coordinates


}
