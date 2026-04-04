#ifndef LASTCARRIAGE_DOORFACTORY_H
#define LASTCARRIAGE_DOORFACTORY_H

#pragma once

#include <SDL3/SDL.h>
#include <string>

#include "../../engine/ecs/Entity.h"
#include "../../engine/resources/AssetManager.h"
#include "../data/AssetPaths.h"

class DoorFactory {
public:
    static Entity& createDoor(Entity& entity,
                              AssetManager& assets,
                              float x,
                              float y,
                              float w,
                              float h,
                              const std::string& targetMap,
                              const std::string& targetSpawn);
};

#endif
