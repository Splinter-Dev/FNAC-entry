#pragma once

#include "raylib.h"
#include <memory>
#include <vector>
#include "light.hh"

struct Entity;

struct World {

    World() = default;
    ~World();

    std::vector<std::unique_ptr<Entity>> entities;
    std::vector<Light> lights;
    /* Entities have to know if they are in shadow mode or not */
    bool shadowMode = false;
};

/* Abstract base class for entities */
struct Entity {
    virtual ~Entity() = default;

    /* Main loop */
    virtual void update(World & world, float delta) = 0;
    virtual void render() const = 0;

    /* Entities that are not visible should return a bounding box that is
     * completely outside the visible area. */
    virtual BoundingBox getBoundingBox() const = 0;
};

void updateWorld(World & world, const float delta);
void renderWorld(World & world, const Camera & camera);
void renderBoundingBoxes(World & world);

void addEntity(World & world, std::unique_ptr<Entity> entity);
bool removeEntity(World & world, Entity * entity);
void clearEntities(World & world);

Light * addLight(World & world, Light && light);
bool removeLight(World & world, Light * light);
void clearLights(World & world);

/* Remove all entities and lights */
void clearWorld(World & world);
