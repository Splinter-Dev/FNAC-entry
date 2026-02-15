#pragma once

#include "world.hh"

bool SerializeEntity(Entity & e, std::ostream& out);
std::unique_ptr<Entity> DeserializeEntity(std::istream& in);

bool SerializeLight(Light & l, std::ostream& out);
bool DeserializeLight(Light & l, std::istream& in);

bool SerializeWorld(World & w, std::string path);
bool DeserializeWorld(World & w, std::string path);
bool LoadWorld(World & w, std::string path);
