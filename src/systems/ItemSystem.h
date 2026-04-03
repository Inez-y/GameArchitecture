/*
 * Pickup and apply item effects
 */
#ifndef LASTCARRIAGE_ITEMSYSTEM_H
#define LASTCARRIAGE_ITEMSYSTEM_H

#pragma once

#include "../ecs/Manager.h"
#include "../GameContext.h"
#include "../components/ItemComponent.h"

class ItemSystem {
public:
    void update(Manager& manager, GameContext& context);

private:
    void applyItemEffect(GameContext& context, class Entity& player, ItemType type);
    static bool overlaps(const SDL_FRect& a, const SDL_FRect& b);
};

#endif