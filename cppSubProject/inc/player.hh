#pragma once

#include <raylib.h>
#include "movingEntity.hh"

class PlayerShadow;

class Player : public MovingEntity {
public:
    Player(Model * model, Texture2D * texture, float scale = 0.5f);

    void update(World & world, float delta) override;
    void render() const override;

    void setShadowMode(bool shadowMode);
    void toggleShadowMode();
    bool shadowMode() const;
    float getRadius() const;

    void setVelocity(float velocity);
    void setDirection(Vector3 direction);

private:
    // Shadow
    PlayerShadow * shadow = nullptr;
    bool shadowModeActive = false;
    float shadowRadius = 10.0f;
};

class PlayerShadow : public MovingEntity {
public:
    PlayerShadow(Model * model, 
            Texture2D * texture, 
            float scale = 0.5f,
            Player * player = nullptr
            );

    void update(World & world, float delta) override;
private:
    Player * player;
};
