#ifndef LASTCARRIAGE_ENEMYFACTORY_H
#define LASTCARRIAGE_ENEMYFACTORY_H

#pragma once

#include <SDL3/SDL.h>

#include "../../engine/ecs/Entity.h"
#include "../../game/components/gameplay/EnemyAIComponent.h"
#include "../../engine/resources/AssetManager.h"
#include "../data/AssetPaths.h"
#include "../../game/components/core/AnimationComponent.h"

class EnemyFactory {
public:
    static Entity& createEnemy(Entity& entity,
                               AssetManager& assets,
                               const std::string& typeName,
                               float startX,
                               float startY,
                               float patrolLeft,
                               float patrolRight);
    

};

#endif //LASTCARRIAGE_ENEMYFACTORY_H