/*
 * Door dverlap and stage change request
 */
#ifndef LASTCARRIAGE_DOORSYSTEM_H
#define LASTCARRIAGE_DOORSYSTEM_H

#pragma once

#include "../../../GameContext.h"
#include "../../../engine/ecs/Manager.h"

class DoorSystem {
public:
    void update(Manager& manager, GameContext& context, float dt);

private:
    static bool overlaps(const SDL_FRect& a, const SDL_FRect& b);
};
#endif