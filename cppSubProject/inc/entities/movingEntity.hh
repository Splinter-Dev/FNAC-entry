#pragma once

#include <raylib.h>
#include "world.hh"
#include "entityFactory.hh"

class MovingEntity : public Entity {

public:

    MovingEntity(
            const std::string & modelPath, 
            const std::string & texturePath, 
            float scale = 1.0f);

    MovingEntity() : model(nullptr), texture(nullptr), scale(1.0f) {}
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

    void jump();
    void setGravity(float gravity);
    void setJumpHeight(float height);

    Matrix getMatrix(Vector3 position, Vector3 direction, float scale) const;
    void drawEntity(Matrix transform) const;

    void setup();

    Model * model = nullptr;
    Texture2D * texture = nullptr;
    std::string modelPath;
    std::string texturePath;

    // Always do this for serialization
    REGISTER(MovingEntity)

    protected:

    Vector3 position = { 0.0f, 0.0f, 0.0f };
    Vector3 direction = { 0.0f, 0.0f, 0.0f };
    Vector3 lastDirection = { 0.0f, 0.0f, 0.0f };
    float velocity = 10.0f;
    float hitboxRadius = 0.5f;

    BoundingBox bb;

    float scale;

    float verticalVelocity = 0.0f;
    float gravity = 30.0f;      // customizable
    float jumpHeight = 3.0f;    // customizable
    bool grounded = false;
    float groundZ = 0.0f;

    float getGravity() const;
    float getJumpHeight() const;
    bool isGrounded() const;

    uint32_t getId() const override;
    std::vector<Attribute> getAttributes() const override;

};
