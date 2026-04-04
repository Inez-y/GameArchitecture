#ifndef LASTCARRIAGE_ITEMFACTORY_H
#define LASTCARRIAGE_ITEMFACTORY_H

#pragma once

#include <SDL3/SDL.h>
#include <string>

#include "../../engine/ecs/Entity.h"
#include "../../engine/resources/AssetManager.h"
#include "../data/AssetPaths.h"

class ItemFactory {
public:
    static Entity& createItem(Entity& entity,
                              AssetManager& assets,
                              const std::string& typeName,
                              float x,
                              float y);
};

#endif //LASTCARRIAGE_ITEMFACTORY_H