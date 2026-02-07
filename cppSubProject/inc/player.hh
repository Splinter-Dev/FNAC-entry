#pragma once

#include <raylib.h>
#include "world.hh"

class Player : public Entity {
public:
    Player();

    void update(World & world, float delta) override;
    void render() override;
    BoundingBox getBoundingBox() const override;
    ~Player() override;

    void setPosition(Vector3 position);
    void addPosition(Vector3 position);
    void setDirection(Vector3 direction);
    void addDirection(Vector3 direction);
    void setVelocity(float velocity);

    void setDirectionX(float x) { direction.x = x; }
    void setDirectionY(float y) { direction.y = y; }
    void setDirectionZ(float z) { direction.z = z; }
    void normalizeDirection();

    void setShadowMode(bool shadowMode);
    void toggleShadowMode();

    Vector3 getPosition();
    Vector3 getDirection();
    float getVelocity();
    bool shadowMode();


private:

    Vector3 position;
    Vector3 direction;
    float velocity;

    // Shadow
    bool shadowModeActive;
    float shadowRadius;
    Vector3 shadowPosition;

    Model model;
    BoundingBox bb;
    const float scale;
};
