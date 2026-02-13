#include <raylib.h>
#include <algorithm>
#include "collision.hh"
#include <raymath.h>

CollisionInfo GetAABBCollision(const BoundingBox& a, const BoundingBox& b)
{
    CollisionInfo result{};
    result.entity = nullptr;
    result.normal = { 0.0f, 0.0f, 0.0f };
    result.penetration = 0.0f;

    // Compute overlap on each axis
    float overlapX = std::min(a.max.x, b.max.x) - std::max(a.min.x, b.min.x);
    if (overlapX <= 0.0f) return result;

    float overlapY = std::min(a.max.y, b.max.y) - std::max(a.min.y, b.min.y);
    if (overlapY <= 0.0f) return result;

    float overlapZ = std::min(a.max.z, b.max.z) - std::max(a.min.z, b.min.z);
    if (overlapZ <= 0.0f) return result;

    // Compute centers
    Vector3 centerA = {
        (a.min.x + a.max.x) * 0.5f,
        (a.min.y + a.max.y) * 0.5f,
        (a.min.z + a.max.z) * 0.5f
    };

    Vector3 centerB = {
        (b.min.x + b.max.x) * 0.5f,
        (b.min.y + b.max.y) * 0.5f,
        (b.min.z + b.max.z) * 0.5f
    };

    // Choose axis of minimum penetration
    if (overlapX < overlapY && overlapX < overlapZ)
    {
        result.penetration = overlapX;
        result.normal = {
            (centerA.x < centerB.x) ? -1.0f : 1.0f,
            0.0f,
            0.0f
        };
    }
    else if (overlapY < overlapZ)
    {
        result.penetration = overlapY;
        result.normal = {
            0.0f,
            (centerA.y < centerB.y) ? -1.0f : 1.0f,
            0.0f
        };
    }
    else
    {
        result.penetration = overlapZ;
        result.normal = {
            0.0f,
            0.0f,
            (centerA.z < centerB.z) ? -1.0f : 1.0f
        };
    }

    return result;
}

CollisionInfo GetCylinderCollision(const Cylinder& a, const Cylinder& b)
{
    CollisionInfo result{};
    result.entity = nullptr;
    result.normal = {0,0,0};
    result.penetration = 0.0f;

    // --- Y extents ---
    float aMinY = a.center.y - a.height * 0.5f;
    float aMaxY = a.center.y + a.height * 0.5f;
    float bMinY = b.center.y - b.height * 0.5f;
    float bMaxY = b.center.y + b.height * 0.5f;

    float overlapY = std::min(aMaxY, bMaxY) - std::max(aMinY, bMinY);
    if (overlapY <= 0.0f)
        return result;

    // --- XZ distance ---
    float dx = a.center.x - b.center.x;
    float dz = a.center.z - b.center.z;

    float distSq = dx*dx + dz*dz;
    float radiusSum = a.radius + b.radius;

    if (distSq >= radiusSum * radiusSum)
        return result;

    float dist = sqrtf(distSq);

    // Radial penetration (side collision amount)
    float overlapXZ = radiusSum - dist;

    // --- Choose axis of minimum penetration ---
    if (overlapY < overlapXZ)
    {
        // Vertical collision (top/bottom)
        if (a.center.y > b.center.y)
            result.normal = {0.0f, 1.0f, 0.0f};
        else
            result.normal = {0.0f, -1.0f, 0.0f};

        result.penetration = overlapY;
    }
    else
    {
        // Side collision
        if (dist < 0.0001f)
        {
            // Perfectly aligned centers
            result.normal = {1.0f, 0.0f, 0.0f};
            result.penetration = radiusSum;
        }
        else
        {
            result.normal = { dx / dist, 0.0f, dz / dist };
            result.penetration = overlapXZ;
        }
    }

    return result;
}

CollisionInfo GetAABBCylinderCollision(const Cylinder& a, const BoundingBox& b)
{
    CollisionInfo info{};
    info.entity = nullptr;
    info.normal = {0.0f, 0.0f, 0.0f};
    info.penetration = 0.0f;

    // --------------------------------------------------
    // 1) Vertical overlap (Y axis)
    // --------------------------------------------------
    float cylMinY = a.center.y;
    float cylMaxY = a.center.y + a.height;

    float overlapY =
        std::min(cylMaxY, b.max.y) -
        std::max(cylMinY, b.min.y);

    if (overlapY <= 0.0f)
        return info; // no collision

    // --------------------------------------------------
    // 2) Horizontal (XZ plane)
    // --------------------------------------------------
    float closestX = Clamp(a.center.x, b.min.x, b.max.x);
    float closestZ = Clamp(a.center.z, b.min.z, b.max.z);

    float dx = a.center.x - closestX;
    float dz = a.center.z - closestZ;

    float distSq = dx * dx + dz * dz;
    float radiusSq = a.radius * a.radius;

    if (distSq > radiusSq)
        return info; // no collision

    // --------------------------------------------------
    // 3) Compute horizontal penetration
    // --------------------------------------------------
    float dist = std::sqrt(distSq);

    Vector3 horizNormal;
    float horizPenetration;

    if (dist > 0.0001f)
    {
        // normal from box -> cylinder
        horizNormal = { dx / dist, 0.0f, dz / dist };
        horizPenetration = a.radius - dist;
    }
    else
    {
        // Cylinder center projected inside the box:
        // choose smallest push axis
        float left   = a.center.x - b.min.x;
        float right  = b.max.x - a.center.x;
        float back   = a.center.z - b.min.z;
        float front  = b.max.z - a.center.z;

        float minPen = left;
        horizNormal = { -1.0f, 0.0f, 0.0f };

        if (right < minPen)
        {
            minPen = right;
            horizNormal = { 1.0f, 0.0f, 0.0f };
        }
        if (back < minPen)
        {
            minPen = back;
            horizNormal = { 0.0f, 0.0f, -1.0f };
        }
        if (front < minPen)
        {
            minPen = front;
            horizNormal = { 0.0f, 0.0f, 1.0f };
        }

        horizPenetration = a.radius + minPen;
    }

    // --------------------------------------------------
    // 4) Choose smallest penetration axis
    // --------------------------------------------------
    if (overlapY < horizPenetration)
    {
        // vertical resolution
        float cylMidY = (cylMinY + cylMaxY) * 0.5f;
        float boxMidY = (b.min.y + b.max.y) * 0.5f;

        info.normal = (cylMidY > boxMidY)
            ? Vector3{0.0f, 1.0f, 0.0f}
            : Vector3{0.0f, -1.0f, 0.0f};

        info.penetration = overlapY;
    }
    else
    {
        // horizontal resolution
        info.normal = horizNormal;
        info.penetration = horizPenetration;
    }

    return info;
}

CollisionInfo GetCollision(const Entity& a, const Entity& b)
{
    CollisionInfo result{};
    result.entity = nullptr;
    result.normal = {0,0,0};
    result.penetration = 0.0f;

    auto hitA = a.getHitBox();
    auto hitB = b.getHitBox();

    if (!hitA || !hitB)
        return result;

    // --- AABB vs AABB ---
    if (std::holds_alternative<AABB>(*hitA) &&
        std::holds_alternative<AABB>(*hitB))
    {
        result = GetAABBCollision(
            std::get<AABB>(*hitA),
            std::get<AABB>(*hitB));
    }

    // --- Cylinder vs Cylinder ---
    else if (std::holds_alternative<Cylinder>(*hitA) &&
             std::holds_alternative<Cylinder>(*hitB))
    {
        result = GetCylinderCollision(
            std::get<Cylinder>(*hitA),
            std::get<Cylinder>(*hitB));
    }

    // --- Cylinder (A) vs AABB (B) ---
    else if (std::holds_alternative<Cylinder>(*hitA) &&
             std::holds_alternative<AABB>(*hitB))
    {
        result = GetAABBCylinderCollision(
            std::get<Cylinder>(*hitA),
            std::get<AABB>(*hitB));
    }

    // --- AABB (A) vs Cylinder (B) ---
    else if (std::holds_alternative<AABB>(*hitA) &&
             std::holds_alternative<Cylinder>(*hitB))
    {
        // Reuse existing function by flipping arguments
        result = GetAABBCylinderCollision(
            std::get<Cylinder>(*hitB),
            std::get<AABB>(*hitA));

        // Flip normal so it still pushes A out of B
        result.normal.x *= -1.0f;
        result.normal.y *= -1.0f;
        result.normal.z *= -1.0f;
    }

    // Attach entity only if collision exists
    if (result.penetration > 0.0f)
        result.entity = const_cast<Entity*>(&b);

    return result;
}
