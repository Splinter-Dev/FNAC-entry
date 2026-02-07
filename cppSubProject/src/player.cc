#include <raylib.h>
#include <raymath.h>
#include "player.hh"
#include "rlgl.h"
#include "raymath.h"
#include "raylib.h"

Player::Player() :
    scale(0.1f)
{

    position = Vector3{ 0.0f, 0.0f, 0.0f };
    direction = Vector3{ 0.0f, 0.0f, 0.0f };
    shadowPosition = Vector3{ 0.0f, 0.0f, 0.0f };
    velocity = 10.0f;
    shadowRadius = 10.0f;
    shadowModeActive = false;

    model = LoadModel("resources/person.obj");

    /* Calculate Bounding Box for the model */
    bb = GetModelBoundingBox(model);

    bb.min.x *= scale;
    bb.min.y *= scale;
    bb.min.z *= scale;

    bb.max.x *= scale;
    bb.max.y *= scale;
    bb.max.z *= scale;
}

void Player::update(World & world, float delta) {

    world.shadowMode = shadowModeActive;

    if (shadowModeActive) {
        Vector3 oldPosition = shadowPosition;

        shadowPosition.x += direction.x * velocity * delta;
        shadowPosition.y += direction.y * velocity * delta;
        shadowPosition.z += direction.z * velocity * delta;

        if (!CheckCollisionPointCircle(
                    (Vector2) { shadowPosition.x, shadowPosition.z },
                    (Vector2) { position.x, position.z },
                    shadowRadius)) {
            shadowPosition = oldPosition;
        }
                    
    } else {
        position.x += direction.x * velocity * delta;
        position.y += direction.y * velocity * delta;
        position.z += direction.z * velocity * delta;
    }
}

void Player::render() {

    Vector3 dir = direction;
    Vector3 rotationAxis = Vector3{ 0.0f, 1.0f, 0.0f };
    float rotationAngle = 0.0f; // default

    if (Vector3Length(dir) > 0.0001f) {
        // atan2 returns radians, convert to degrees
        rotationAngle = atan2f(dir.x, dir.z) * (180.0f / PI);
    }

    if (shadowModeActive) {
        // Draw the original model with rotation 0.0f
        DrawModelEx(model, position, (Vector3){0, 1, 0}, 0.0f,
                (Vector3){scale, scale, scale}, RED);

        DrawModelEx(model, shadowPosition, rotationAxis, rotationAngle,
                (Vector3){scale, scale, scale}, BLACK);

        // Draw a circle on the ground indicating the shadow radius
        DrawCircle3D(position, shadowRadius, (Vector3){1, 0, 0}, 90.0f, RED);
    } else {
        DrawModelEx(model, position, (Vector3){0, 1, 0}, rotationAngle,
                (Vector3){scale, scale, scale}, RED);
    }
}

BoundingBox Player::getBoundingBox() const {
    BoundingBox b = bb;
    Vector3 position = shadowModeActive ? shadowPosition : this->position;
    b.min.x += position.x;
    b.min.y += position.y;
    b.min.z += position.z;
    b.max.x += position.x;
    b.max.y += position.y;
    b.max.z += position.z;
    return b;
}

void Player::setPosition(Vector3 position) {
    this->position = position;
}

void Player::addPosition(Vector3 position) {
    if (shadowModeActive) {
        shadowPosition.x += position.x;
        shadowPosition.y += position.y;
        shadowPosition.z += position.z;
    } else {
        this->position.x += position.x;
        this->position.y += position.y;
        this->position.z += position.z;
    }
}

void Player::setDirection(Vector3 direction) {
    this->direction = direction;
}

void Player::addDirection(Vector3 direction) {
    if (shadowModeActive) {
        shadowPosition.x += direction.x;
        shadowPosition.y += direction.y;
        shadowPosition.z += direction.z;
    } else {
        this->direction.x += direction.x;
        this->direction.y += direction.y;
        this->direction.z += direction.z;
    }
}

void Player::toggleShadowMode() {
    if (shadowModeActive) {
        shadowModeActive = false;
        shadowPosition = position;
    } else {
        shadowModeActive = true;
        shadowPosition = position;
    }
}

void Player::setVelocity(float velocity) {
    this->velocity = velocity;
}

Vector3 Player::getPosition() {
    return position;
}

Vector3 Player::getDirection() {
    return direction;
}

float Player::getVelocity() {
    return velocity;
}

bool Player::shadowMode() {
    return shadowModeActive;
}

void Player::normalizeDirection() {
    direction = Vector3Normalize(direction);
}

Player::~Player() {
    UnloadModel(model);
}
