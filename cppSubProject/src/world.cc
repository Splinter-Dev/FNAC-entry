#include "world.hh"
#include "shaderManager.hh"
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

void World::addEntity(std::unique_ptr<Entity> entity) {
    entities.emplace_back(std::move(entity));
}

void addEntityToWorld(World & world, std::unique_ptr<Entity> entity) {
    world.entitiesToAdd.emplace_back(std::move(entity));
}

bool World::removeEntity(Entity * entity) {
    // Check if entity is in the list
    auto it = std::find_if(entities.begin(), entities.end(),
            [entity](const std::unique_ptr<Entity>& e) {
                return e.get() == entity;
            });
    if (it == entities.end()) return false;

    entitiesToRemove.emplace_back(entity);
    return true;
}

void World::applyPendingChanges() {
    // Add entities
    for (auto & entity : entitiesToAdd) {
        entities.emplace_back(std::move(entity));
    }
    entitiesToAdd.clear();

    // Remove entities
    for (auto & entity : entitiesToRemove) {
        entities.erase(
            std::remove_if(entities.begin(), entities.end(),
                [entity](const std::unique_ptr<Entity>& e) {
                    return e.get() == entity;
                }),
            entities.end()
        );
    }
    entitiesToRemove.clear();
}

Light * addLightToWorld(World & world, Light && light) {
    world.lights.emplace_back(std::move(light));
    return &world.lights.back();
}

void clearWorldLights(World & world) {
    world.lights.clear();
}

void clearWorld(World & world) {
    clearWorldEntities(world);
    clearWorldLights(world);
    world.entitiesToAdd.clear();
    world.entitiesToRemove.clear();
}

bool removeLightInWorld(World &world, Light *light) {
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
    world.applyPendingChanges();

    for (auto & entity : world.entities) {
        // Some entities might add / romove other entities 
        // in the world directly
        if (entity != nullptr) {
            entity->update(world, delta);
        }
    }
}

void renderWorld(World & world, const Camera & camera) {

    SM.setAmbient(world.ambient);
    SM.setCamera(camera);
    SM.sendLights(world.lights);

    for (auto & entity : world.entities) {
        if (entity != nullptr && 
                isVisible(entity->getBoundingBox(), camera,
                GetScreenWidth(), GetScreenHeight())) {
            entity->render();
        }
    }
}

bool removeEntityInWorld(World &world, Entity* entity) {

    // Check if the entity is in the list
    auto it = std::find_if(world.entities.begin(), world.entities.end(),
            [entity](const std::unique_ptr<Entity>& e) {
                return e.get() == entity;
            });
    if (it == world.entities.end()) return false;

    world.entitiesToRemove.emplace_back(entity);
    return true;
}

void clearWorldEntities(World & world) {
    world.entities.clear();
}

/* For testing purposes */
void renderBoundingBoxes(World & world) {
    for (auto & entity : world.entities) {
        DrawBoundingBox(entity->getBoundingBox(), RED);
    }
}

World::~World() {
    clearWorld(*this);
}
