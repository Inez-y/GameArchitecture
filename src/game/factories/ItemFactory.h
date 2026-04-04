#ifndef LASTCARRIAGE_ITEMFACTORY_H
#define LASTCARRIAGE_ITEMFACTORY_H

#pragma once

#include "../../engine/ecs/Entity.h"
#include <SDL3/SDL.h>
#include <string>

class ItemFactory {
public:
    static Entity& createItem(Entity& entity,
                              SDL_Renderer* renderer,
                              const std::string& typeName,
                              float x,
                              float y);
};

#endif //LASTCARRIAGE_ITEMFACTORY_H