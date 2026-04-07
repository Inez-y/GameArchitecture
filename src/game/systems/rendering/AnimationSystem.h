#ifndef LASTCARRIAGE_ANIMATIONSYSTEM_H
#define LASTCARRIAGE_ANIMATIONSYSTEM_H

#pragma once

#include "../../../GameContext.h"
#include "../../../engine/ecs/Manager.h"

class AnimationSystem {
public:
    void update(Manager& manager, GameContext& context, float dt);
};

#endif // LASTCARRIAGE_ANIMATIONSYSTEM_HH