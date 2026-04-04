#include "EnemyFactory.h"

#include "../components/identity/EnemyTagComponent.h"
#include "../components/core/TransformComponent.h"
#include "../components/core/SpriteComponent.h"
#include "../components/core/PhysicsComponent.h"
#include "../components/core/HealthComponent.h"
#include "../components/gameplay/EnemyAIComponent.h"

static EnemyType stringToEnemyType(const std::string& type) {
    if (type == "Patrol") return EnemyType::Patrol;
    if (type == "Shooter") return EnemyType::Shooter;
    if (type == "Flying") return EnemyType::Flying;
    if (type == "Boss") return EnemyType::Boss;
    return EnemyType::Patrol;
}

static const char* enemyTexturePath(EnemyType type) {
    switch (type) {
        case EnemyType::Boss:    return "../assets/boss1.png";
        case EnemyType::Shooter: return "../assets/enemy.png";
        case EnemyType::Flying:  return "../assets/enemy.png";
        case EnemyType::Patrol:
        default:                 return "../assets/enemy.png";
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

    entity.addComponent<EnemyTagComponent>();
    entity.addComponent<TransformComponent>(startX, startY, 32.0f, 32.0f);
    entity.addComponent<SpriteComponent>(assets.getTexture(enemyTexturePath(type)));
    entity.addComponent<PhysicsComponent>(100.0f, 900.0f, 450.0f, type != EnemyType::Flying);
    entity.addComponent<HealthComponent>(type == EnemyType::Boss ? 20 : 3);

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

    return entity;
}