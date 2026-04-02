#ifndef LASTCARRIAGE_PLAYERFACTORY_H
#define LASTCARRIAGE_PLAYERFACTORY_H

#pragma once

#include "../ecs/Entity.h"
#include <SDL3/SDL.h>

class PlayerFactory {
public:
    static Entity& createPlayer(Entity& playerEntity,
                                SDL_Renderer* renderer,
                                float startX,
                                float startY);
};

#endif //LASTCARRIAGE_PLAYERFACTORY_H