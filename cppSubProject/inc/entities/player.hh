#pragma once

#include <raylib.h>
#include "movingEntity.hh"

class PlayerShadow;

class Player : public MovingEntity {
public:
    Player(Model * model, Texture2D * texture, float scale = 0.5f);

    void update(World & world, float delta) override;
    void render() const override;

    void toggleShadowMode();
    void setShadowMode(bool shadowMode);
    void setVelocity(float velocity);
    void setDirection(Vector3 direction);
    void resolveCollision(const CollisionInfo& info) override;

    void jump();

    void setRadius(float radius);
    void addToRadius(float delta);
    float getRadius() const;
    bool shadowMode() const;

private:
    // Shadow
    PlayerShadow * shadow = nullptr;
    Light * light = nullptr; // Light that follows the player
    bool shadowModeActive = false;
    float shadowRadius = 10.0f;
};

class PlayerShadow : public MovingEntity {
public:
    PlayerShadow(
            Model * model, 
            Texture2D * texture, 
            float scale = 0.5f,
            Player * player = nullptr
            );

    void update(World & world, float delta) override;
    void resolveCollision(const CollisionInfo& info) override;

private:
    Player * player;
};
