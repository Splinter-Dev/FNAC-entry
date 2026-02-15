#pragma once

#include <vector>
#include <string>
#include <memory>
#include <cassert>
#include <cstdint>
#include "world.hh"

#define REGISTER(T) \
    inline static constexpr const char* TYPE_NAME = #T; \
    inline static constexpr uint32_t TYPE_ID = hash32(TYPE_NAME); \
    static AutoRegister<T> reg;

typedef std::unique_ptr<Entity> (*CreateFn)();

class EntityFactory {
public:
    using CreateFn = std::unique_ptr<Entity>(*)();

    struct TypeInfo {
        uint32_t id;
        std::string name;
        CreateFn create;
    };

    static void registerType(uint32_t id, const std::string& name, CreateFn fn);
    static std::unique_ptr<Entity> create(uint32_t id);
    static const TypeInfo* getInfo(uint32_t id);
    static uint32_t idFromName(const std::string& name);
    static std::vector<TypeInfo> allTypes();
    static std::string idToString(uint32_t id);

    // Return a string describing all registered entity types
    static std::string toString();
};

//---------------------------------------
// Hash function (FNV-1a 32-bit)
constexpr uint32_t hash32(const char* str, uint32_t h = 2166136261u) {
    return *str ? hash32(str + 1, (h ^ static_cast<uint32_t>(*str)) * 16777619u) : h;
}

// Generic creator template
template<typename T>
std::unique_ptr<Entity> createEntity() {
    return std::make_unique<T>();
}

template<typename T>
struct AutoRegister {
    AutoRegister() {
        EntityFactory::registerType(
            T::TYPE_ID,
            T::TYPE_NAME,
            &createEntity<T>
        );
    }
};

template<typename T>
void forceRegister()
{
    static AutoRegister<T> reg;
}

// Make sure all entities are registered
void RegisterAllEntities();
