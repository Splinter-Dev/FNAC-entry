#include "world.hh"
#include "shaderManager.hh"
#include <algorithm>
#include <raymath.h>
#include <vector>
#include <raylib.h>
#include <algorithm>
#include "collision.hh"

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

static bool isVisibleCircle(const Vector3& center,
                            float radius,
                            const Camera& cam,
                            float screenWidth,
                            float screenHeight)
{
    // Camera rectangle in world space
    float camLeft   = cam.position.x - screenWidth  / 2.0f;
    float camRight  = cam.position.x + screenWidth  / 2.0f;
    float camTop    = cam.position.z - screenHeight / 2.0f;
    float camBottom = cam.position.z + screenHeight / 2.0f;

    // Circle bounds in XZ plane
    float minX = center.x - radius;
    float maxX = center.x + radius;
    float minZ = center.z - radius;
    float maxZ = center.z + radius;

    // Outside test (same idea as AABB)
    return !(maxX < camLeft  ||
             minX > camRight ||
             maxZ < camTop   ||
             minZ > camBottom);
}

static std::vector<Light> filterLightsByVisible(
    const std::vector<Light>& lights,
    const Camera& cam,
    float screenWidth,
    float screenHeight)
{
    std::vector<Light> result;
    result.reserve(lights.size()); // pre-allocate to avoid repeated reallocations

    for (const auto& l : lights) {
        if (isVisibleCircle(l.position, l.radius, cam, screenWidth, screenHeight)) {
            result.push_back(l); // copy the visible light
        }
    }

    return result;
}

void World::addEntity(std::unique_ptr<Entity> entity) {
    entities.emplace_back(std::move(entity));
}

void addEntityToWorld(World & world, std::unique_ptr<Entity> entity, bool immediately) {
    world.entitiesToAdd.emplace_back(std::move(entity));
    if (immediately) world.applyPendingChanges();
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
    resolveCollisions(world);

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

    std::vector<Light> visibleLights = filterLightsByVisible(
            world.lights,
            camera,
            GetScreenWidth(),
            GetScreenHeight());

    SM.sendLights(visibleLights);

    for (auto & entity : world.entities) {
        if (entity != nullptr && 
                isVisible(entity->getBoundingBox(), camera,
                GetScreenWidth(), GetScreenHeight())) {
            entity->render();
        }
    }
}

bool removeEntityInWorld(World &world, Entity* entity, bool immediately) {
    // Check if the entity is in the list
    auto it = std::find_if(world.entities.begin(), world.entities.end(),
            [entity](const std::unique_ptr<Entity>& e) {
                return e.get() == entity;
            });
    if (it == world.entities.end()) return false;

    world.entitiesToRemove.emplace_back(entity);

    if (immediately) world.applyPendingChanges();
    return true;
}

void clearWorldEntities(World & world) {
    world.entities.clear();
}

/* For testing purposes */
void renderBoundingBoxes(World & world) {
    for (auto & entity : world.entities)
        DrawBoundingBox(entity->getBoundingBox(), RED);
}

void renderHitBoxes(World & world) {
    for (auto & entity : world.entities) {
        if (entity->getHitBox().has_value()) {
            if (std::holds_alternative<AABB>(entity->getHitBox().value()))
                DrawBoundingBox(std::get<AABB>(entity->getHitBox().value()), RED);
            else if (std::holds_alternative<Cylinder>(entity->getHitBox().value())) {
                Cylinder cylinder = std::get<Cylinder>(entity->getHitBox().value());
                DrawCylinderWires(
                    cylinder.center,
                    cylinder.radius,
                    cylinder.radius,
                    cylinder.height,
                    10,
                    RED);
            }
        }
    }
}

void resolveCollisions(World & world) {
    // Simple collision resolution, should probably add
    // a collision grid to speed up the process
    for (auto & entity : world.entities) {
        for (auto & other : world.entities) {
            if (entity != other && 
                entity->getHitBox().has_value() && 
                other->getHitBox().has_value()) {

                CollisionInfo info = GetCollision(*entity, *other);

                if (info.penetration > 0.0f) {
                    info.entity = other.get();
                    entity->resolveCollision(info);
                }
            }
        }
    }
}

World::~World() {
    clearWorld(*this);
}
