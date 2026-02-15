#include "serialization/binaryStream.hh"
#include "serialization/binaryStream.hh"
#include "entityFactory.hh"

#define WORLD_FILE_SIGNATURE 0x69420420

bool SerializeEntity(Entity& e, std::ostream& out)
{
    // Write entity ID
    if (!writeU32(out, e.getId()))
        return false;

    // Write attributes
    for (auto& attr : e.getAttributes())
    {
        void* ptr = attr.getPtr(e);

        switch (attr.type)
        {
            case AttributeType::Float:
            {
                float* f = static_cast<float*>(ptr);
                if (!writeF32(out, *f)) return false;
                break;
            }
            case AttributeType::Int:
            {
                uint32_t* i = static_cast<uint32_t*>(ptr);
                if (!writeU32(out, *i)) return false;
                break;
            }
            case AttributeType::Bool:
            {
                bool* b = static_cast<bool*>(ptr);
                if (!writeBool(out, *b)) return false;
                break;
            }
            case AttributeType::Short:
            {
                uint16_t* s = static_cast<uint16_t*>(ptr);
                if (!writeU16(out, *s)) return false;
                break;
            }
            case AttributeType::String:
            {
                std::string* str = static_cast<std::string*>(ptr);
                if (!writeString(out, *str)) return false;
                break;
            }
            case AttributeType::Vec3:
            {
                Vector3* v = static_cast<Vector3*>(ptr);
                if (!writeF32(out, v->x)) return false;
                if (!writeF32(out, v->y)) return false;
                if (!writeF32(out, v->z)) return false;
                break;
            }
        }
    }

    return true;
}

std::unique_ptr<Entity> DeserializeEntity(std::istream& in)
{
    // Read entity ID
    uint32_t id;
    if (!readU32(in, id))
        return nullptr;

    // Create correct entity type via factory
    std::unique_ptr<Entity> e = EntityFactory::create(id);
    if (!e) return nullptr;

    // Read all attributes
    for (const auto& attr : e->getAttributes())
    {
        void* ptr = attr.getPtr(*e);

        switch (attr.type)
        {
            case AttributeType::Short:
            {
                uint16_t v;
                if (!readU16(in, v)) return nullptr;
                *static_cast<uint16_t*>(ptr) = v;
                break;
            }

            case AttributeType::Int:
            {
                uint32_t v;
                if (!readU32(in, v)) return nullptr;
                *static_cast<uint32_t*>(ptr) = v;
                break;
            }

            case AttributeType::Float:
            {
                float v;
                if (!readF32(in, v)) return nullptr;
                *static_cast<float*>(ptr) = v;
                break;
            }

            case AttributeType::Bool:
            {
                bool v;
                if (!readBool(in, v)) return nullptr;
                *static_cast<bool*>(ptr) = v;
                break;
            }

            case AttributeType::String:
            {
                auto& s = *static_cast<std::string*>(ptr);
                if (!readString(in, s)) return nullptr;
                break;
            }

            case AttributeType::Vec3:
            {
                Vector3* v = static_cast<Vector3*>(ptr);
                if (!readF32(in, v->x)) return nullptr;
                if (!readF32(in, v->y)) return nullptr;
                if (!readF32(in, v->z)) return nullptr;
                break;
            }
        }
    }

    return e;
}

bool SerializeLight(Light & l, std::ostream& out)
{
    if (!writeU32(out, (uint32_t)l.type)) return false;

    if (!writeBool(out, l.enabled)) return false;

    if (!writeU8(out, l.color.r)) return false;
    if (!writeU8(out, l.color.g)) return false;
    if (!writeU8(out, l.color.b)) return false;
    if (!writeU8(out, l.color.a)) return false;

    if (!writeF32(out, l.position.x)) return false;
    if (!writeF32(out, l.position.y)) return false;
    if (!writeF32(out, l.position.z)) return false;

    if (!writeF32(out, l.target.x)) return false;
    if (!writeF32(out, l.target.y)) return false;
    if (!writeF32(out, l.target.z)) return false;

    if (!writeF32(out, l.intensity)) return false;
    if (!writeF32(out, l.radius)) return false;
    if (!writeF32(out, l.specular)) return false;

    return true;
}

bool DeserializeLight(Light & l, std::istream& in)
{

    uint32_t type;
    if (!readU32(in, type)) return false;
    l.type = static_cast<LightType>(type);

    if (!readBool(in, l.enabled)) return false;

    if (!readU8(in, l.color.r)) return false;
    if (!readU8(in, l.color.g)) return false;
    if (!readU8(in, l.color.b)) return false;
    if (!readU8(in, l.color.a)) return false;

    if (!readF32(in, l.position.x)) return false;
    if (!readF32(in, l.position.y)) return false;
    if (!readF32(in, l.position.z)) return false;

    if (!readF32(in, l.target.x)) return false;
    if (!readF32(in, l.target.y)) return false;
    if (!readF32(in, l.target.z)) return false;

    if (!readF32(in, l.intensity)) return false;
    if (!readF32(in, l.radius)) return false;
    if (!readF32(in, l.specular)) return false;

    return true;
}

#include <fstream>

bool SerializeWorld(World & w, std::string path)
{
    std::ofstream out(path, std::ios::binary);
    if (!out.is_open())
    {
        TraceLog(LOG_ERROR, "Failed to open world file: %s", path.c_str());
        return false;
    }

    // Write the world signature
    if (!writeU32(out, WORLD_FILE_SIGNATURE)) {
        TraceLog(LOG_ERROR, "Failed to write world signature");
        return false;
    }

    uint32_t numEntities = w.entities.size();
    if (!writeU32(out, numEntities)) {
        TraceLog(LOG_ERROR, "Failed to write number of entities");
        return false;
    }

    for (auto & e : w.entities)
    {
        if (!SerializeEntity(*e, out)) {
            TraceLog(LOG_ERROR, "Failed to serialize entity");
            return false;
        }
    }

    uint32_t numLights = w.lights.size();
    if (!writeU32(out, numLights)) {
        TraceLog(LOG_ERROR, "Failed to write number of lights");
        return false;
    }

    for (auto & l : w.lights)
    {
        if (!SerializeLight(l, out)) {
            TraceLog(LOG_ERROR, "Failed to serialize light");
            return false;
        }
    }

    return true;
}


bool DeserializeWorld(World & w, std::string path)
{
    std::ifstream in(path, std::ios::binary);
    if (!in.is_open())
    {
        TraceLog(LOG_ERROR, "Failed to open world file: %s", path.c_str());
        return false;
    }

    uint32_t signature;
    if (!readU32(in, signature)) return false;

    if (signature != WORLD_FILE_SIGNATURE)
    {
        TraceLog(LOG_ERROR, "Invalid world file signature");
        return false;
    }

    uint32_t numEntities;
    if (!readU32(in, numEntities)) return false;

    for (uint32_t i = 0; i < numEntities; ++i)
    {
        auto e = DeserializeEntity(in);
        if (!e) {
            TraceLog(LOG_ERROR, "Failed to deserialize entity");
            return false;
        }
        w.entities.push_back(std::move(e));
    }

    uint32_t numLights;
    if (!readU32(in, numLights)) return false;

    for (uint32_t i = 0; i < numLights; ++i)
    {
        Light l;
        if (!DeserializeLight(l, in)) {
            TraceLog(LOG_ERROR, "Failed to deserialize light");
            return false;
        }
        w.lights.push_back(l);
    }

    return true;
}
