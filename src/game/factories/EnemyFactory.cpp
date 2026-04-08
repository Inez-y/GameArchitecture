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
    if (type == "Boss" || type == "BossPatrol" || type == "BossShooter") return EnemyType::Boss;
    return EnemyType::Patrol;
}

static const AnimationPresets::EnemyAnimationSet& enemyAnimationSet(const std::string& typeName) {
    if (typeName == "Shooter") {
        return AnimationPresets::EnemyShooter;
    }
    if (typeName == "Flying") {
        return AnimationPresets::EnemyFlying;
    }
    if (typeName == "BossShooter") {
        return AnimationPresets::BossShooter;
    }
    if (typeName == "Boss" || typeName == "BossPatrol") {
        return AnimationPresets::BossPatrol;
    }
    return AnimationPresets::EnemyPatrol;
}

static std::string enemyInitialSpritesheet(const std::string& typeName) {
    return enemyAnimationSet(typeName).idle.spritesheetPath;
}

Entity& EnemyFactory::createEnemy(Entity& entity,
                                  AssetManager& assets,
                                  const std::string& typeName,
                                  float startX,
                                  float startY,
                                  float patrolLeft,
                                  float patrolRight) {
    const EnemyType type = stringToEnemyType(typeName);
    const bool isBoss = (type == EnemyType::Boss);
    const auto& animSet = enemyAnimationSet(typeName);

    entity.addComponent<EnemyTagComponent>();

    entity.addComponent<TransformComponent>(
        startX,
        startY,
        static_cast<float>(animSet.idle.frameWidth),
        static_cast<float>(animSet.idle.frameHeight)
    );

    entity.addComponent<SpriteComponent>(
        assets.getTexture(enemyInitialSpritesheet(typeName))
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
