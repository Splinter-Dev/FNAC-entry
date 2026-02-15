#pragma once

#include "world.hh"
#include "scene.hh"
#include <sstream>

#include "raygui.h"

struct HeaderButton {
    std::string text;
    std::function<void()> callback;
};

struct Editor {

    void init();
    void render();

    World world;

    std::vector <HeaderButton> headerButtons;

    std::vector<Entity *> selectedEntities;
    std::vector<Light *> selectedLights;
    bool showGrid = true;

    // Stores serialized entities / lights
    std::stringstream clipboard;

    Scene scene;
    bool running = true;

    private:


    Vector2 dragStart = { 0.0f, 0.0f };
    Vector2 dragDelta = { 0.0f, 0.0f };
    bool dragging = false;

    bool selecting = false;
    Rectangle selectionRect = { 0.0f, 0.0f, 0.0f, 0.0f };

    bool renderHitBoxes = false;
    bool renderAABBs = false;

    bool popup = false;
    std::string popupWarningMessage;

    void popupWarning(const std::string & message);

    void handleKeys();
};
