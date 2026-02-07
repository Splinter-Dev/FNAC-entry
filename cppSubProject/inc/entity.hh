#pragma once

#include "raylib.h"
#include "world.hh"

class Entity {

public:
    Entity();
    ~Entity();

    virtual void update(World & world, float delta);
    void render(World & world);

    void setPosition(Vector3 position);
    void addPosition(Vector3 position);

    void setDirection(Vector3 direction);
    void setDirectionX(float x);
    void setDirectionY(float y);
    void setDirectionZ(float z);

    void setVelocity(float velocity);
    void setShadowMode(bool shadowMode);

    BoundingBox getBoundingBox();

private:    
    // Only update / render if shadow mode is enabled
    bool shadowMode = false;
    Vector3 position;
    Vector3 direction;
};  
