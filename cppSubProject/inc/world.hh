#pragma once

#include "raylib.h"
#include <memory>
#include <vector>

struct Entity;

struct World {
    std::vector<std::unique_ptr<Entity>> entities;
    bool shadowMode;
    Color backgroundColor;
};

struct Entity {
    virtual void update(World & world, float delta) = 0;
    virtual void render() = 0;
    virtual BoundingBox getBoundingBox() const = 0;
    virtual ~Entity() = default;
};

World newWorld();
void addEntity(World & world, std::unique_ptr<Entity> entity);
void removeEntity(World & world, std::unique_ptr<Entity> entity);
void clearEntities(World & world);
void updateWorld(World & world, const float delta);
void renderWorld(World & world, const Camera & camera);
void renderBoundingBoxes(World & world);
void setBackgroundColor(World & world, const Color & color);
