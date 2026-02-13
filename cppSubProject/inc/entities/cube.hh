#include "world.hh"

class Cube : public Entity {
    public:
        Cube(Vector3 position, float height, float width);
        void update(World & world, float delta);
        void render() const;

        AABB getBoundingBox() const;

        std::optional<HitBox> getHitBox() const;
        void resolveCollision(const CollisionInfo& info);

        Vector3 getPosition() const;
        void setPosition(Vector3 position);

    private:
        Vector3 position;
        float height;
        float width;
};
