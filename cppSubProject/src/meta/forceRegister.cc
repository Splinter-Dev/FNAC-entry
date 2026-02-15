#include "entities/movingEntity.hh"
#include "entities/cube.hh"
#include "entityFactory.hh"

// I'll make a script to generate this file
// at compile time

static bool registered = false;

void RegisterAllEntities() {
    if (registered) return;
    forceRegister<MovingEntity>();
    forceRegister<Cube>();
    registered = true;
}
