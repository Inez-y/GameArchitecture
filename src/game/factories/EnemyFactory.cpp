#include "EnemyFactory.h"

#include "../components/identity/EnemyTagComponent.h"
#include "../components/core/TransformComponent.h"
#include "../components/core/SpriteComponent.h"
#include "../components/core/PhysicsComponent.h"
#include "../components/core/HealthComponent.h"
#include "../components/core/AnimationComponent.h"
#include "../components/gameplay/EnemyAIComponent.h"
#include "../data/AssetPaths.h"
#include "../data/AnimationClips.h"

static EnemyType stringToEnemyType(const std::string& type) {
    if (type == "Patrol") return EnemyType::Patrol;
    if (type == "Shooter") return EnemyType::Shooter;
    if (type == "Flying") return EnemyType::Flying;
    if (type == "Boss") return EnemyType::Boss;
    return EnemyType::Patrol;
}

static const AnimationPresets::EnemyAnimationSet& enemyAnimationSet(EnemyType type) {
    switch (type) {
        case EnemyType::Boss:
            return AnimationPresets::Boss;
        case EnemyType::Shooter:
        case EnemyType::Flying:
        case EnemyType::Patrol:
        default:
            return AnimationPresets::Enemy;
    }
}

static const char* enemyInitialSpritesheet(EnemyType type) {
    switch (type) {
        case EnemyType::Boss:
            return AssetPaths::BOSS_IDLE_SPRITESHEET;
        case EnemyType::Shooter:
        case EnemyType::Flying:
        case EnemyType::Patrol:
        default:
            return AssetPaths::ENEMY_IDLE_SPRITESHEET;
    }
}

Entity& EnemyFactory::createEnemy(Entity& entity,
                                  AssetManager& assets,
                                  const std::string& typeName,
                                  float startX,
                                  float startY,
                                  float patrolLeft,
                                  float patrolRight) {
    EnemyType type = stringToEnemyType(typeName);
    const bool isBoss = (type == EnemyType::Boss);
    const auto& animSet = enemyAnimationSet(type);

    entity.addComponent<EnemyTagComponent>();

    // Use idle clip size as the initial source-frame size.
    // You can scale this down if the art is too large in-game.
    entity.addComponent<TransformComponent>(
        startX,
        startY,
        static_cast<float>(animSet.idle.frameWidth),
        static_cast<float>(animSet.idle.frameHeight)
    );

    entity.addComponent<SpriteComponent>(
        assets.getTexture(enemyInitialSpritesheet(type))
    );

    entity.addComponent<PhysicsComponent>(100.0f, 900.0f, 450.0f, type != EnemyType::Flying);
    entity.addComponent<HealthComponent>(isBoss ? 20 : 3);

    auto& ai = entity.addComponent<EnemyAIComponent>(type, patrolLeft, patrolRight);

    if (type == EnemyType::Boss) {
        ai.state = EnemyState::Idle;
        ai.attackRange = 60.0f;
        ai.chaseRange = 260.0f;
    } else if (type == EnemyType::Shooter) {
        ai.state = EnemyState::Idle;
        ai.attackRange = 250.0f;
        ai.chaseRange = 300.0f;
    } else if (type == EnemyType::Flying) {
        ai.state = EnemyState::Chase;
        ai.attackRange = 60.0f;
        ai.chaseRange = 260.0f;
    } else {
        ai.state = EnemyState::Patrol;
        ai.attackRange = 40.0f;
        ai.chaseRange = 220.0f;
    }

    auto& anim = entity.addComponent<AnimationComponent>();
    AnimationPresets::applySet(anim, animSet);

    return entity;
}