#include "PlayerFactory.h"

#include "../components/identity/PlayerTagComponent.h"
#include "../components/core/TransformComponent.h"
#include "../components/core/SpriteComponent.h"
#include "../components/core/PhysicsComponent.h"
#include "../components/core/HealthComponent.h"
#include "../components/core/InputComponent.h"
#include "../components/core/AnimationComponent.h"
#include "../components/gameplay/WeaponComponent.h"
#include "../data/AssetPaths.h"
#include "../data/AnimationClips.h"

Entity& PlayerFactory::createPlayer(Entity& entity,
                                    AssetManager& assets,
                                    float startX,
                                    float startY) {
    entity.addComponent<PlayerTagComponent>();
    // Raw frame size = 228 x 192
    entity.addComponent<TransformComponent>(startX, startY, 228.0f, 192.0f);

    // Start with idle sheet; AnimationSystem should swap textures later.
    entity.addComponent<SpriteComponent>(
        assets.getTexture(AssetPaths::PLAYER_IDLE_SPRITESHEET)
    );

    entity.addComponent<PhysicsComponent>(200.0f);
    entity.addComponent<HealthComponent>(10);
    entity.addComponent<WeaponComponent>(6, 1.2f);
    entity.addComponent<InputComponent>();

    auto& anim = entity.addComponent<AnimationComponent>();
    AnimationPresets::applySet(anim, AnimationPresets::Player);

    return entity;
}