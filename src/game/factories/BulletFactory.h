#ifndef LASTCARRIAGE_BULLETFACTORY_H
#define LASTCARRIAGE_BULLETFACTORY_H

#pragma once

#include <SDL3/SDL.h>
#include "../../engine/ecs/Entity.h"
#include "../components/gameplay/BulletComponent.h"
#include "../../engine/resources/AssetManager.h"
#include "../data/AssetPaths.h"

class BulletFactory {
public:
    static Entity& createBullet(Entity& entity,
                                AssetManager& assets,
                                BulletOwner owner,
                                float startX,
                                float startY,
                                int direction);
};

#endif //LASTCARRIAGE_BULLETFACTORY_H