/*
 * Bullets, damage, combat interactions
 */
#ifndef LASTCARRIAGE_COMBATSYSTEM_H
#define LASTCARRIAGE_COMBATSYSTEM_H

#pragma once

#include "../../../GameContext.h"
#include "../../../engine/ecs/Manager.h"

class CombatSystem {
public:
    void update(Manager& manager, GameContext& context, float dt);

private:
    void queueEnemyBulletSpawns(Manager& manager, GameContext& context);
    void spawnQueuedBullets(Manager& manager, GameContext& context);
    void updateBullets(Manager& manager, GameContext& context, float dt);
    void handleBulletCollisions(Manager& manager, GameContext& context);
    void handleEnemyContactDamage(Manager& manager, GameContext& context);

    static bool overlaps(const SDL_FRect& a, const SDL_FRect& b);
};

#endif