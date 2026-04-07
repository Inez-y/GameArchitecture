#ifndef LASTCARRIAGE_PLAYERFACTORY_H
#define LASTCARRIAGE_PLAYERFACTORY_H

#pragma once

#include "../../engine/ecs/Entity.h"
#include "../../engine/resources/AssetManager.h"
#include "../../game/data/AssetPaths.h"
#include "../../game/components/core/AnimationComponent.h"
#include <SDL3/SDL.h>

class PlayerFactory {
public:
    static Entity& createPlayer(Entity& playerEntity,
                                AssetManager& assets,
                                float startX,
                                float startY);
};

#endif //LASTCARRIAGE_PLAYERFACTORY_H