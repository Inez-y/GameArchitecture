/*
 * Player/enemy movement and camera update
 */
#ifndef LASTCARRIAGE_MOVEMENTSYSTEM_H
#define LASTCARRIAGE_MOVEMENTSYSTEM_H
#pragma once

#include "../../../GameContext.h"
#include "../../../engine/ecs/Manager.h"

class MovementSystem {
public:
    void update(Manager& manager, GameContext& context, float dt);
    void updateCamera(GameContext& context);

private:
    void updatePlayer(GameContext& context, float dt);
    void updateEnemies(Manager& manager, GameContext& context, float dt);
};

#endif //LASTCARRIAGE_MOVEMENTSYSTEM_H