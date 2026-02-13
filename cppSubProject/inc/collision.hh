#pragma once

#include <raylib.h>
#include "world.hh"

struct CollisionInfo {
    Entity * entity;
    Vector3 normal;
    float penetration;
};

CollisionInfo GetAABBCollision(const BoundingBox& a, const BoundingBox& b);
CollisionInfo GetCylinderCollision(const Cylinder& a, const Cylinder& b);
CollisionInfo GetAABBCylinderCollision(const Cylinder& a, const BoundingBox& b);
CollisionInfo GetCollision(const Entity& a, const Entity& b);
