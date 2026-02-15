#include <raylib.h>
#include <raymath.h>
#include "entities/cube.hh"
#include "shaderManager.hh"

// AutoRegister<Cube> Cube::reg;
Cube::Cube(Vector3 position, float height, float width) :
    position(position),
    height(height),
    width(width)
{}

void Cube::update(World & world, float delta) {
    // Do notn
}

void Cube::render() const {
    BeginShaderMode(SM.getToon());
    DrawCube(position, width, height, width, WHITE);
    EndShaderMode();
}

AABB Cube::getBoundingBox() const {
    AABB aabb;
    aabb.min.x = position.x - width / 2.0f;
    aabb.min.y = position.y - height / 2.0f;
    aabb.min.z = position.z - width / 2.0f;
    aabb.max.x = position.x + width / 2.0f;
    aabb.max.y = position.y + height / 2.0f;
    aabb.max.z = position.z + width / 2.0f;
    return aabb;
}

std::optional<HitBox> Cube::getHitBox() const {
    return getBoundingBox();
}

void Cube::resolveCollision(const CollisionInfo& info) {
    // Do notn
}

std::vector<Attribute> Cube::getAttributes() const {
    return {
        MakeAttribute("height", &Cube::height, AttributeType::Float),
        MakeAttribute("width", &Cube::width, AttributeType::Float),
        MakeAttribute("position", &Cube::position, AttributeType::Vec3),
    };
}

void Cube::setPosition(Vector3 position) {
    this->position = position;
}

Vector3 Cube::getPosition() const {
    return position;
}

uint32_t Cube::getId() const {
    return TYPE_ID;
}
