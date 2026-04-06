/**
 * Patrol/Shooter/Flying enemy state transitions
 * Boss movement/attack movement state transitions
 */
#ifndef LASTCARRIAGE_ENEMYMOVEMENTLOGIC_H
#define LASTCARRIAGE_ENEMYMOVEMENTLOGIC_H

#pragma once

#include "../../components/core/TransformComponent.h"
#include "../../components/core/PhysicsComponent.h"
#include "../../components/core/HealthComponent.h"
#include "../../components/gameplay/EnemyAIComponent.h"

class EnemyMovementLogic {
public:
    static void updateEnemy(TransformComponent& transform,
                            PhysicsComponent& physics,
                            HealthComponent& health,
                            EnemyAIComponent& ai,
                            const TransformComponent& playerTransform,
                            float dt);

private:
    static void updatePatrolEnemy(TransformComponent& transform,
                                  PhysicsComponent& physics,
                                  HealthComponent& health,
                                  EnemyAIComponent& ai,
                                  const TransformComponent& playerTransform,
                                  float dt);

    static void updateShooterEnemy(TransformComponent& transform,
                                   PhysicsComponent& physics,
                                   HealthComponent& health,
                                   EnemyAIComponent& ai,
                                   const TransformComponent& playerTransform,
                                   float dt);

    static void updateFlyingEnemy(TransformComponent& transform,
                                  PhysicsComponent& physics,
                                  HealthComponent& health,
                                  EnemyAIComponent& ai,
                                  const TransformComponent& playerTransform,
                                  float dt);

    static void updateBossEnemy(TransformComponent& transform,
                                PhysicsComponent& physics,
                                HealthComponent& health,
                                EnemyAIComponent& ai,
                                const TransformComponent& playerTransform,
                                float dt);

    static float distanceToPlayer(const TransformComponent& enemyTransform,
                                  const TransformComponent& playerTransform,
                                  EnemyAIComponent& ai);
};

#endif // LASTCARRIAGE_ENEMYMOVEMENTLOGIC_HRRIAGE_ENEMYMOVEMENTLOGIC_H