#include "world.hh"
#include <algorithm>
#include <vector>

static bool isVisible(const BoundingBox& box, const Camera& cam, float screenWidth, float screenHeight) {
    // For top-down, ignore Y (height) and just check X,Z bounds
    float minX = box.min.x, maxX = box.max.x;
    float minZ = box.min.z, maxZ = box.max.z;

    // camera center + screen half size
    float camLeft   = cam.position.x - screenWidth/2;
    float camRight  = cam.position.x + screenWidth/2;
    float camTop    = cam.position.z - screenHeight/2;
    float camBottom = cam.position.z + screenHeight/2;

    // check if box is outside camera bounds
    return !(maxX < camLeft || minX > camRight || maxZ < camTop || minZ > camBottom);
}

World newWorld() {
    World world;
    world.shadowMode = false;
    world.backgroundColor = WHITE;
    return world;
}

void addEntity(World & world, std::unique_ptr<Entity> entity) {
    world.entities.push_back(std::move(entity));
}

void updateWorld(World & world, const float delta) {
    for (auto & entity : world.entities) {
        entity->update(world, delta);
    }
}

void renderWorld(World & world, const Camera & camera) {
    ClearBackground(world.backgroundColor);

    for (auto & entity : world.entities) {
        if (isVisible(entity->getBoundingBox(), camera,
            GetScreenWidth(), GetScreenHeight())) {
            entity->render();
        }
    }
}

void setBackgroundColor(World & world, const Color & color) {
    world.backgroundColor = color;
}

void removeEntity(World & world, Entity* entity) {
    auto & vec = world.entities;
    vec.erase(
        std::remove_if(vec.begin(), vec.end(),
            [entity](const std::unique_ptr<Entity>& e) {
                return e.get() == entity; // match the pointer
            }),
        vec.end()
    );
}

void clearEntities(World & world) {
    world.entities.clear();     // destroys all unique_ptrs, deleting entities
}

void renderBoundingBoxes(World & world) {
    for (auto & entity : world.entities) {
        DrawBoundingBox(entity->getBoundingBox(), RED);
    }
}
