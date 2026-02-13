#pragma once

#include <raylib.h>
#include "world.hh"

class MovingEntity : public Entity {

public:

    MovingEntity(Model * model, Texture2D * texture, float scale = 0.5f);
    ~MovingEntity() override;

    void update(World & world, float delta) override;
    void render() const override;
    std::optional<HitBox> getHitBox() const override;
    BoundingBox getBoundingBox() const override;
    void resolveCollision(const CollisionInfo& info) override;

    void setPosition(Vector3 position) override;
    void addPosition(Vector3 position);
    void setDirection(Vector3 direction);
    void addDirection(Vector3 direction);
    void setVelocity(float velocity);
    void setHitboxRadius(float radius);
    void normalizeDirection();

    Vector3 getPosition() const override;
    Vector3 getDirection() const;
    float getVelocity() const;
    float getScale() const;

    Model * model;
    Texture2D * texture;

protected:

    Matrix getMatrix(Vector3 position, Vector3 direction, float scale) const;
    void drawEntity(Matrix transform) const;

    Vector3 position = { 0.0f, 0.0f, 0.0f };
    Vector3 direction = { 0.0f, 0.0f, 0.0f };
    Vector3 lastDirection = { 0.0f, 0.0f, 0.0f };
    float velocity = 10.0f;
    float hitboxRadius = 0.5f;

    BoundingBox bb;
    const float scale;

    // --- Jumping ---
    private:
    float verticalVelocity = 0.0f;
    float gravity = 30.0f;      // customizable
    float jumpHeight = 3.0f;    // customizable
    bool grounded = false;

    float groundZ = 0.0f;

    public:
    void jump();
    void setGravity(float gravity);
    void setJumpHeight(float height);

    float getGravity() const;
    float getJumpHeight() const;
    bool isGrounded() const;
};
