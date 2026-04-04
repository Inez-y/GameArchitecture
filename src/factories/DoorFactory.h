#ifndef LASTCARRIAGE_DOORFACTORY_H
#define LASTCARRIAGE_DOORFACTORY_H

#pragma once

#include "../ecs/Entity.h"
#include <SDL3/SDL.h>
#include <string>

class DoorFactory {
public:
    static Entity& createDoor(Entity& entity,
                              SDL_Renderer* renderer,
                              float x,
                              float y,
                              float w,
                              float h,
                              const std::string& targetMap,
                              const std::string& targetSpawn);
};

#endif
