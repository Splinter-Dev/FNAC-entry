#pragma once

#include "raylib.h"
#include <memory>
#include <vector>
#include <optional>
#include <variant>
#include "light.hh"

struct Entity;

struct World {

    World() = default;
    ~World();

    std::vector<std::unique_ptr<Entity>> entities;
    std::vector<Light> lights;

    /* Apply pending adds / removals to the world */
    void applyPendingChanges();
    std::vector<std::unique_ptr<Entity>> entitiesToAdd;
    std::vector<Entity *> entitiesToRemove;

    /* Entities have to know if they are in shadow mode or not */
    bool shadowMode = false;

    /* ambient 0.3f, 0.3f, 0.3f, 1.0f */
    Color ambient = { 76, 76, 76, 255 };

    private:

    /* Entities are only added in the end of the update loop,
     * these add / romove the entities from entities / lights */
    void addEntity(std::unique_ptr<Entity> entity);
    bool removeEntity(Entity * entity);
};

struct Cylinder {
    Vector3 center;
    float radius;
    float height;
};

#define HitBox std::variant<BoundingBox, Cylinder>
#define AABB BoundingBox

struct CollisionInfo;

/* Abstract base class for entities */
struct Entity {
    virtual ~Entity() = default;

    /* Main loop */
    virtual void update(World & world, float delta) = 0;
    virtual void render() const = 0;

    /* Entities that are not visible should return a bounding box that is
     * completely outside the visible area. */
    virtual AABB getBoundingBox() const = 0;

    /* Collisions */
    virtual std::optional<HitBox> getHitBox() const = 0;
    virtual void resolveCollision(const CollisionInfo& info) = 0;

    /* Used in editor */
    virtual Vector3 getPosition() const = 0;
    virtual void setPosition(Vector3 position) = 0;
};

void updateWorld(World & world, const float delta);
void renderWorld(World & world, const Camera & camera);
void renderBoundingBoxes(World & world);
void renderHitBoxes(World & world);

void addEntityToWorld(World & world, std::unique_ptr<Entity> entity);
bool removeEntityInWorld(World & world, Entity * entity);
void clearWorldEntities(World & world);

Light * addLightToWorld(World & world, Light && light);
bool removeLightInWorld(World & world, Light * light);
void clearWorldLights(World & world);

void resolveCollisions(World & world);

/* Remove all entities and lights */
void clearWorld(World & world);
