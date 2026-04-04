#ifndef LASTCARRIAGE_BULLETFACTORY_H
#define LASTCARRIAGE_BULLETFACTORY_H

#pragma once

#include "../../engine/ecs/Entity.h"
#include <SDL3/SDL.h>
#include "../components/gameplay/BulletComponent.h"

class BulletFactory {
public:
    static Entity& createBullet(Entity& entity,
                                SDL_Renderer* renderer,
                                BulletOwner owner,
                                float startX,
                                float startY,
                                int direction);
};

#endif //LASTCARRIAGE_BULLETFACTORY_H