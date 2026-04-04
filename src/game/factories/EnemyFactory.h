#ifndef LASTCARRIAGE_ENEMYFACTORY_H
#define LASTCARRIAGE_ENEMYFACTORY_H

#pragma once

#include "../../engine/ecs/Entity.h"
#include <SDL3/SDL.h>
#include "../../game/components/gameplay/EnemyAIComponent.h"

class EnemyFactory {
public:
    static Entity& createEnemy(Entity& entity,
                               SDL_Renderer* renderer,
                               const std::string& typeName,
                               float startX,
                               float startY,
                               float patrolLeft,
                               float patrolRight);
};

#endif //LASTCARRIAGE_ENEMYFACTORY_H