#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include <raymath.h>
#include <raylib.h>
#include <rlgl.h>
#include "transform.hpp"
#include "entities/movingEntity.hh"
#include "collision.hh"

MovingEntity::MovingEntity(Model * model, Texture2D * texture, float scale) :
    scale(scale)
{
    position = Vector3 { 0.0f, 0.0f, 0.0f };
    direction = Vector3 { 0.0f, 0.0f, 0.0f };
    velocity = 10.0f;

    this->model = model;
    this->texture = texture;

    for (int i = 0; i < model->materialCount; i++)
        SetMaterialTexture(&model->materials[i], MATERIAL_MAP_DIFFUSE, *texture);

    /* Calculate Bounding Box for the model */
    bb = GetModelBoundingBox(*model);
}

Matrix MovingEntity::getMatrix(Vector3 position, 
                               Vector3 direction, 
                               float scale) const {
    float rotY = 0.0f;

    if (Vector3Length(direction) > 0.0001f)
        rotY = atan2f(direction.x, direction.z) * (180.0f / PI);

    Matrix rotX = MatrixRotateX(DEG2RAD * -90.0f);
    Matrix rotYMat = MatrixRotateZ(DEG2RAD * rotY + DEG2RAD * 180.0f);
    Matrix rotation = MatrixMultiply(rotYMat, rotX);
    Matrix transform = MatrixMultiply(rotation, MatrixScale(scale, scale, scale));
    Matrix translation = MatrixTranslate(position.x, position.y, position.z);

    return MatrixMultiply(transform, translation);
}

void MovingEntity::drawEntity(Matrix transform) const {
    DrawModelTransformed(*model, transform);
}

void MovingEntity::update(World & world, float delta) {

    grounded = false;

    position.x += direction.x * velocity * delta;
    // Reserved for jumping
    // position.y += direction.y * velocity * delta;
    position.z += direction.z * velocity * delta;

    // --- Vertical physics (jump / gravity) ---
    if (!grounded)
    {
        verticalVelocity -= gravity * delta;
        position.y += verticalVelocity * delta;

        // Ground collision (cannot fall below Z = 0)
        if (position.y <= groundZ)
        {
            position.y = groundZ;
            verticalVelocity = 0.0f;
            grounded = true;
        }
    }
    groundZ = 0.0f;
}

void MovingEntity::render() const {
    Vector3 dir = Vector3Equals(direction, {0.0f, 0.0f, 0.0f}) ? lastDirection : direction;
    Matrix transform = getMatrix(position, dir, scale);
    drawEntity(transform);
}

BoundingBox MovingEntity::getBoundingBox() const {
    Vector3 dir = Vector3Equals(direction, {0.0f, 0.0f, 0.0f}) ? lastDirection : direction;
    Matrix transform = getMatrix(position, dir, scale);
    return TransformBoundingBox(bb, transform);
}

void MovingEntity::setPosition(Vector3 position) {
    this->position = position;
}

void MovingEntity::addPosition(Vector3 position) {
    this->position.x += position.x;
    this->position.y += position.y;
    this->position.z += position.z;
}

void MovingEntity::setDirection(Vector3 direction) {
    if (!Vector3Equals(direction, {0.0f, 0.0f, 0.0f})) {
        lastDirection = direction;
    }
    this->direction = direction;
    normalizeDirection();
}

void MovingEntity::addDirection(Vector3 direction) {
    this->direction.x += direction.x;
    this->direction.y += direction.y;
    this->direction.z += direction.z;
    if (!Vector3Equals(this->direction, {0.0f,0.0f,0.0f})) {
        lastDirection = this->direction;
    }
}

void MovingEntity::setVelocity(float velocity) {
    this->velocity = velocity;
}

Vector3 MovingEntity::getPosition() const {
    return position;
}

Vector3 MovingEntity::getDirection() const {
    return direction;
}

float MovingEntity::getVelocity() const {
    return velocity;
}

float MovingEntity::getScale() const {
    return scale;
}

void MovingEntity::setHitboxRadius(float radius) {
    hitboxRadius = radius;
}

std::optional<HitBox> MovingEntity::getHitBox() const {
    BoundingBox aabb = getBoundingBox();
    float height = aabb.max.y - aabb.min.y;
    Cylinder cylinder = {
        getPosition(),
        hitboxRadius,
        height
    };
    return cylinder;
}

void MovingEntity::resolveCollision(const CollisionInfo& info)
{
    if (info.entity == nullptr || info.entity == this) return;
    if (info.penetration <= 0.0f) return;

    // ----------------------------------------------------
    // Determine collision type
    // ----------------------------------------------------
    const float GROUND_NORMAL_THRESHOLD = 0.2f;
    bool isGround = (info.normal.y > GROUND_NORMAL_THRESHOLD);

    // ----------------------------------------------------
    // Positional correction
    // ----------------------------------------------------
    if (isGround)
    {
        // Ground: resolve vertically only.
        // This prevents sideways pushes when landing.
        position.y += info.penetration;

        // Track highest valid ground this frame
        groundZ = std::max(groundZ, position.y);

        // Stop falling
        if (verticalVelocity < 0.0f)
            verticalVelocity = 0.0f;
    }
    else
    {
        // Walls / ceilings: full separation
        position.x += info.normal.x * info.penetration;
        position.y += info.normal.y * info.penetration;
        position.z += info.normal.z * info.penetration;
    }
}

void MovingEntity::normalizeDirection() {
    direction = Vector3Normalize(direction);
}

void MovingEntity::jump()
{
    if (!grounded) return;

    // v = sqrt(2gh)
    verticalVelocity = sqrtf(2.0f * gravity * jumpHeight);
    grounded = false;
}

void MovingEntity::setGravity(float gravity)
{
    this->gravity = gravity;
}

void MovingEntity::setJumpHeight(float height)
{ 
    jumpHeight = height; 
}

float MovingEntity::getGravity() const
{ return gravity; }

float MovingEntity::getJumpHeight() const
{ return jumpHeight; }

bool MovingEntity::isGrounded() const
{ return grounded; }

MovingEntity::~MovingEntity() {}
