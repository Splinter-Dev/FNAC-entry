#include "world.hh"
#include "entityFactory.hh"

class Cube : public Entity {

    public:
        Cube(Vector3 position, float height, float width);
        Cube() : position({0.0f, 0.0f, 0.0f}), height(0.0f), width(0.0f) {}

        // API
        void update(World & world, float delta) override;
        void render() const override;
        AABB getBoundingBox() const override;
        std::optional<HitBox> getHitBox() const override;
        void resolveCollision(const CollisionInfo& info) override;
        Vector3 getPosition() const override;
        void setPosition(Vector3 position) override;
        uint32_t getId() const override;
        std::vector<Attribute> getAttributes() const override;
        REGISTER(Cube)

    private:
        Vector3 position;
        float height;
        float width;
};
