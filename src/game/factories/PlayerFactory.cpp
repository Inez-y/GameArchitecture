#include "PlayerFactory.h"

#include "../components/identity/PlayerTagComponent.h"
#include "../components/core/TransformComponent.h"
#include "../components/core/SpriteComponent.h"
#include "../components/core/PhysicsComponent.h"
#include "../components/core/HealthComponent.h"
#include "../components/gameplay/WeaponComponent.h"
#include "../components/core/InputComponent.h"

Entity& PlayerFactory::createPlayer(Entity& entity,
                                    SDL_Renderer* renderer,
                                    float startX,
                                    float startY) {
    entity.addComponent<PlayerTagComponent>();
    entity.addComponent<TransformComponent>(startX, startY, 32.0f, 32.0f);
    entity.addComponent<SpriteComponent>(renderer, "../assets/player.png");
    entity.addComponent<PhysicsComponent>(200.0f);
    entity.addComponent<HealthComponent>(10);
    entity.addComponent<WeaponComponent>(6, 1.2f);
    entity.addComponent<InputComponent>();

    return entity;
}