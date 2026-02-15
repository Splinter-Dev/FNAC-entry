#include <memory>
#include <string>
#include <cstdint>
#include <unordered_map>
#include "world.hh"
#include "entityFactory.hh"

namespace {
    std::unordered_map<uint32_t, EntityFactory::TypeInfo>& byId() {
        static std::unordered_map<uint32_t, EntityFactory::TypeInfo> map;
        return map;
    }

    std::unordered_map<std::string, uint32_t>& idByName() {
        static std::unordered_map<std::string, uint32_t> map;
        return map;
    }
}

void EntityFactory::registerType(uint32_t id, const std::string& name, CreateFn fn) {
    TraceLog(LOG_INFO, "Registering %s (%u)", name.c_str(), id);
    assert(byId().count(id) == 0 && "Duplicate TYPE_ID");
    assert(idByName().count(name) == 0 && "Duplicate TYPE_NAME");
    TypeInfo info{id, name, fn};
    byId()[id] = info;
    idByName()[name] = id;
}

std::unique_ptr<Entity> EntityFactory::create(uint32_t id) {
    auto it = byId().find(id);
    if(it == byId().end()) return nullptr;
    return it->second.create();
}

const EntityFactory::TypeInfo* EntityFactory::getInfo(uint32_t id) {
    auto it = byId().find(id);
    if(it == byId().end()) return nullptr;
    return &it->second;
}

uint32_t EntityFactory::idFromName(const std::string& name) {
    auto it = idByName().find(name);
    if(it == idByName().end()) return 0;
    return it->second;
}

std::vector<EntityFactory::TypeInfo> EntityFactory::allTypes() {
    std::vector<TypeInfo> types;
    for(const auto& kv : byId()) types.push_back(kv.second);
    return types;
}

std::string EntityFactory::idToString(uint32_t id) {
    auto it = byId().find(id);
    if(it == byId().end()) return "Unknown";
    return it->second.name;
}

#include <sstream>  // for std::ostringstream
std::string EntityFactory::toString() {
    std::ostringstream oss;
    oss << "EntityFactory state:\n";
    for (const auto& kv : byId()) {
        const TypeInfo& t = kv.second;
        oss << "  ID: " << t.id << " | Name: " << t.name << "\n";
    }
    return oss.str();
}
