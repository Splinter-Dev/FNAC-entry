#include <raylib.h>
#include <raymath.h>
#include <memory>
#include "player.hh"
#include "movingEntity.hh"
#include "world.hh"
#include "shaderManager.hh"

Player::Player(Model * model, Texture2D * texture, float scale) :
    MovingEntity(model, texture, scale)
{
    if (model != nullptr) {
        TraceLog(LOG_ERROR, "Model is null");
        model->materials[0].shader = SM.getToon();
    }
}

void Player::update(World & world, float delta) {
    world.shadowMode = shadowModeActive;

    if (shadowModeActive) {
        if (shadow == nullptr) {
            // Insert shadow entity into the world
            shadow = std::make_unique<PlayerShadow>(
                    model, texture, getScale(), this).release();
            shadow->setPosition(getPosition());
            shadow->setVelocity(getVelocity());
            addEntity(world, std::unique_ptr<Entity>(shadow));
        }
    } else {
        if (shadow != nullptr) {
            // Remove shadow entity from the world
            if (removeEntity(world, shadow)) {
                shadow = nullptr;
            } else {
                TraceLog(LOG_ERROR, "Failed to remove shadow entity");
            }
        }
        MovingEntity::update(world, delta);
    }
}

void Player::render() const {
    MovingEntity::render();
    // Render the circle if in shadow mode
    if (shadowModeActive) {
        DrawCircle3D(getPosition(), getRadius(), (Vector3){1, 0, 0}, 90.0f, RED);
    }
}

void Player::setRadius(float radius) {
    shadowRadius = radius;
}

void Player::addToRadius(float delta) {
    shadowRadius += delta;
}

void Player::setDirection(Vector3 direction) {
    // If shadow mode is active, change the shadow entity's direction
    if (shadowModeActive && shadow != nullptr) {
        shadow->setDirection(direction);
    } else {
        MovingEntity::setDirection(direction);
    }
}

void Player::toggleShadowMode() {
    setShadowMode(!shadowModeActive);
}

void Player::setShadowMode(bool shadowMode) {
    if (shadowModeActive != shadowMode) {
        shadowModeActive = shadowMode;
    }
}

void Player::setVelocity(float velocity) {
    if (shadowModeActive && shadow != nullptr) {
        shadow->setVelocity(velocity);
    } else {
        MovingEntity::setVelocity(velocity);
    }
}

bool Player::shadowMode() const {
    return shadowModeActive;
}

float Player::getRadius() const {
    return shadowRadius;
}

/* Shadow entity */

PlayerShadow::PlayerShadow(Model * model, Texture2D * texture, 
        float scale, Player * player) : 
    MovingEntity(model, texture, scale),
    player(player) {}

void PlayerShadow::update(World & world, float delta) {
    if (world.shadowMode && player != nullptr) {

        Vector3 pos = getPosition();
        float rad = player->getRadius();

        MovingEntity::update(world, delta);

        // If the shadow is outside the player's radius, roll back
        if (!CheckCollisionPointCircle(
                    (Vector2) { 
                    getPosition().x, 
                    getPosition().z 
                    },
                    (Vector2) { 
                    player->getPosition().x,
                    player->getPosition().z
                    },
                    rad)) {
            // Rollback
            setPosition(pos);
        }
    }
}
