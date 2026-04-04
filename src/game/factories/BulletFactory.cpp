#include "BulletFactory.h"

#include "../components/identity/BulletTagComponent.h"
#include "../components/gameplay/BulletComponent.h"
#include "../components/core/TransformComponent.h"
#include "../components/core/SpriteComponent.h"

Entity& BulletFactory::createBullet(Entity& entity,
                                    SDL_Renderer* renderer,
                                    BulletOwner owner,
                                    float startX,
                                    float startY,
                                    int direction) {
    entity.addComponent<BulletTagComponent>();
    entity.addComponent<TransformComponent>(startX, startY, 16.0f, 8.0f);
    entity.addComponent<BulletComponent>(owner, 300.0f, direction, 1);

    // entity.addComponent<SpriteComponent>(renderer, "../assets/bullet.png");

    return entity;
}