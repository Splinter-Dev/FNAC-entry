#include "world.hh"
#include "shader.hh"
#include <algorithm>
#include <vector>

static bool isVisible(const BoundingBox& box, const Camera& cam, float screenWidth, float screenHeight) {
    // For top-down, ignore Y (height) and just check X,Z bounds
    float minX = box.min.x, maxX = box.max.x;
    float minZ = box.min.z, maxZ = box.max.z;

    // camera center + screen half size
    float camLeft   = cam.position.x - screenWidth / 2;
    float camRight  = cam.position.x + screenWidth / 2;
    float camTop    = cam.position.z - screenHeight / 2;
    float camBottom = cam.position.z + screenHeight / 2;

    // check if box is outside camera bounds
    return !(maxX < camLeft || minX > camRight || maxZ < camTop || minZ > camBottom);
}

void addEntity(World & world, std::unique_ptr<Entity> entity) {
    world.entities.push_back(std::move(entity));
}

Light * addLight(World & world, Light && light) {
    world.lights.push_back(std::move(light));
    return &world.lights.back();
}

void clearLights(World & world) {
    world.lights.clear();
}

void clearWorld(World & world) {
    clearEntities(world);
    clearLights(world);
}

bool removeLight(World &world, Light *light) {
    if (light == nullptr) return false;

    auto &vec = world.lights;

    // Find the pointer in the vector
    auto it = std::find_if(vec.begin(), vec.end(),
            [light](const Light &l) { return &l == light; });

    if (it != vec.end()) {
        vec.erase(it);  // remove it safely
        return true;
    }

    return false;
}

void updateWorld(World & world, const float delta) {
    for (auto & entity : world.entities) {
        // Some entities might add / romove other entities 
        // in the world directly
        if (entity != nullptr) {
            entity->update(world, delta);
        }
    }
}

void renderWorld(World & world, const Camera & camera) {

    sendLightsToShader(toonShader, world.lights);

    for (auto & entity : world.entities) {
        if (entity != nullptr && 
                isVisible(entity->getBoundingBox(), camera,
                GetScreenWidth(), GetScreenHeight())) {
            entity->render();
        }
    }
}

bool removeEntity(World &world, Entity* entity) {
    auto &vec = world.entities;

    // Count before removal
    size_t before = vec.size();

    // Remove the entity whose pointer matches
    vec.erase(
        std::remove_if(vec.begin(), vec.end(),
            [entity](const std::unique_ptr<Entity>& e) {
                return e.get() == entity;
            }),
        vec.end()
    );

    // Return true if something was removed
    return vec.size() < before;
}

void clearEntities(World & world) {
    world.entities.clear();     // destroys all unique_ptrs, deleting entities
}

/* For testing purposes */
void renderBoundingBoxes(World & world) {
    for (auto & entity : world.entities) {
        DrawBoundingBox(entity->getBoundingBox(), RED);
    }
}

World::~World() {
    TraceLog(LOG_INFO, "World destroyed");
    clearWorld(*this);
}
