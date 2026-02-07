#include "raylib.h"
#include "world.hh"
#include <cmath>

class Bird : public Entity {

public:

    Bird(const char* modelPath, 
            float scale = 1.0f,
            float radius = 5.0f, 
            float speed = 1.0f)
        : orbitRadius(radius), 
        orbitSpeed(speed),
        scale(scale)
    {
        model = LoadModel(modelPath);
        BoundingBox box = GetModelBoundingBox(model);
        position = { radius, 10.0f, 0.0f };
        angle = 0.0f;
    }

    ~Bird() override {
        UnloadModel(model);
    }

    void update(World& world, float delta) override {

        if (!world.shadowMode) { return; }

        // Increase angle over time
        angle += orbitSpeed * delta;

        // Keep angle in a reasonable range
        if (angle > 2.0f * PI) {
            angle -= 2.0f * PI;
        }

        // Compute new position in a circle around (0,0,0)
        position.x = cosf(angle) * orbitRadius;
        position.z = sinf(angle) * orbitRadius;
    }

    void render() override {

        // Rotate the model so it faces the direction of movement
        float degrees = angle * RAD2DEG;

        DrawModelEx(
            model,
            position,
            { 0.0f, 1.0f, 0.0f },   // rotation axis (Y axis)
            -degrees + 90.0f,       // rotate to face forward along path
            Vector3{ scale, scale, scale },   // scale
            WHITE
        );
    }

    BoundingBox getBoundingBox() const override {
        BoundingBox box = boundingBox;

        // Offset the bounding box by current position
        box.min.x += position.x;
        box.min.y += position.y;
        box.min.z += position.z;

        box.max.x += position.x;
        box.max.y += position.y;
        box.max.z += position.z;

        // Scale the bounding box
        box.min.x *= scale;
        box.min.y *= scale;
        box.min.z *= scale;

        box.max.x *= scale;
        box.max.y *= scale;
        box.max.z *= scale;

        return box;
    }

private:
    Model model;
    BoundingBox boundingBox;

    Vector3 position;

    float angle;
    float orbitRadius;
    float orbitSpeed;
    const float scale;
};
