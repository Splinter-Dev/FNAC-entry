#pragma once

#include <raylib.h>
#include "world.hh"

class MovingEntity : public Entity {

public:
    MovingEntity(Model * model, Texture2D * texture, float scale = 0.5f);
    void update(World & world, float delta) override;
    void render() const override;
    BoundingBox getBoundingBox() const override;
    ~MovingEntity() override;

    void setPosition(Vector3 position);
    void addPosition(Vector3 position);
    void setDirection(Vector3 direction);
    void addDirection(Vector3 direction);
    void setVelocity(float velocity);
    void normalizeDirection();

    Vector3 getPosition() const;
    Vector3 getDirection() const;
    float getVelocity() const;
    float getScale() const;

    Model * model;
    Texture2D * texture;
private:

    Matrix getMatrix(Vector3 position, Vector3 direction, float scale) const;
    void drawEntity(Matrix transform) const;

    Vector3 position = { 0.0f, 0.0f, 0.0f };
    Vector3 direction = { 0.0f, 0.0f, 0.0f };
    Vector3 lastDirection = { 0.0f, 0.0f, 0.0f };
    float velocity = 10.0f;

    BoundingBox bb;
    const float scale;
};
