#include <raylib.h>
#include <raymath.h>
#include <memory>
#include "entities/player.hh"
#include "entities/movingEntity.hh"
#include "world.hh"
#include "shaderManager.hh"
#include "collision.hh"

Player::Player(
        const std::string & modelPath, 
        const std::string & texturePath, 
        float scale) :
    MovingEntity(modelPath, texturePath, scale)
{
    if (model != nullptr) {
        /* The entities can use shadermanager for rendering purposes */
        model->materials[0].shader = SM.getToon();
    }
    setHitboxRadius(0.7f);
}

void Player::update(World & world, float delta) {

    world.shadowMode = shadowModeActive;

    if (shadowModeActive) {
        if (shadow == nullptr) {
            auto newShadow = std::make_unique<PlayerShadow>(modelPath, texturePath, scale, this);
            // Insern shadow entity into the world
            shadow = newShadow.get();
            shadow->setPosition(getPosition());
            shadow->setVelocity(getVelocity());
            addEntityToWorld(world, std::move(newShadow));
        }
    } else {
        if (shadow != nullptr) {
            // Remove shadow entity from the world
            if (removeEntityInWorld(world, shadow)) {
                shadow = nullptr;
            }
        }
        MovingEntity::update(world, delta);
    }

    if (light == nullptr) {
        Vector3 pos= getPosition();
        pos.y += 4.0f;
        Light l = pointLight( pos, WHITE, 0.5f, 5.0f);
        light = addLightToWorld(world, std::move(l));
    } else {
        Vector3 pos= getPosition();
        pos.y += 4.0f;
        light->position = pos;
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
PlayerShadow::PlayerShadow(
        const std::string & modelPath, 
        const std::string & texturePath, 
        float scale, 
        Player * player) :
    MovingEntity(modelPath, texturePath, scale),
    player(player) {
        setHitboxRadius(0.7f);
        setJumpHeight(6.0f);
    }

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
            // Rollback x and y
            setPosition(
                    (Vector3){
                        pos.x,
                        this->position.y,
                        pos.z
                    });
        }
    }
}

void Player::jump() {
    if (shadowModeActive && shadow != nullptr) {
        shadow->jump();
    } else{
        MovingEntity::jump();
    }
}

void Player::resolveCollision(const CollisionInfo& info) {
    // If the player is in shadow mode, ignore collisions
    if (shadowModeActive || info.entity == shadow) return;   // Ignore the player
    MovingEntity::resolveCollision(info);
}

void PlayerShadow::resolveCollision(const CollisionInfo& info) {
    if (info.entity == player) return;   // Ignore the player
    MovingEntity::resolveCollision(info);
}
