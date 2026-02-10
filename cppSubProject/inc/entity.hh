#pragma once

#include "raylib.h"
#include "world.hh"

class Decoration : public Entity {
public:
    Decoration(Model model, Vector3 position, Vector3 scale)
        : m_model(model), m_position(position), m_scale(scale) {
            // Calculate bounding box
            bb = GetModelBoundingBox(m_model);
            bb.min.x = m_position.x + bb.min.x * m_scale.x;
            bb.min.y = m_position.y + bb.min.y * m_scale.y;
            bb.min.z = m_position.z + bb.min.z * m_scale.z;
            bb.max.x = m_position.x + bb.max.x * m_scale.x;
            bb.max.y = m_position.y + bb.max.y * m_scale.y;
            bb.max.z = m_position.z + bb.max.z * m_scale.z;
        }

    void update(World& world, float delta) override {
        // Decorations are static; nothing to update
    }

    void render() override {
        DrawModelEx(
            m_model,
            m_position,
            {0.0f, 1.0f, 0.0f}, // Y-axis rotation
            0.0f,               // No rotation by default
            m_scale,
            WHITE               // Color tint
        );
    }

    BoundingBox getBoundingBox() const override { return bb; }

private:
    Model m_model;
    Vector3 m_position;
    Vector3 m_scale;
    BoundingBox bb;
};
