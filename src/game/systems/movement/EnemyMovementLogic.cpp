#include "EnemyMovementLogic.h"

#include <cstdlib>

float EnemyMovementLogic::distanceToPlayer(const TransformComponent& enemyTransform,
                                          const TransformComponent& playerTransform,
                                          EnemyAIComponent& ai) {
    return ai.distanceTo(enemyTransform.x, enemyTransform.y,
                         playerTransform.x, playerTransform.y);
}

void EnemyMovementLogic::updateEnemy(TransformComponent& transform,
                                     PhysicsComponent& physics,
                                     HealthComponent& health,
                                     EnemyAIComponent& ai,
                                     const TransformComponent& playerTransform,
                                     float dt) {
    if (health.isDead()) {
        ai.changeState(EnemyState::Dead);
    }

    switch (ai.type) {
        case EnemyType::Patrol:
            updatePatrolEnemy(transform, physics, health, ai, playerTransform, dt);
            break;
        case EnemyType::Shooter:
            updateShooterEnemy(transform, physics, health, ai, playerTransform, dt);
            break;
        case EnemyType::Flying:
            updateFlyingEnemy(transform, physics, health, ai, playerTransform, dt);
            break;
        case EnemyType::Boss:
            updateBossEnemy(transform, physics, health, ai, playerTransform, dt);
            break;
    }

    if (health.isDead()) {
        ai.changeState(EnemyState::Dead);
    }
}

void EnemyMovementLogic::updatePatrolEnemy(TransformComponent& transform,
                                           PhysicsComponent& physics,
                                           HealthComponent& health,
                                           EnemyAIComponent& ai,
                                           const TransformComponent& playerTransform,
                                           float dt) {
    const float dist = distanceToPlayer(transform, playerTransform, ai);

    switch (ai.state) {
        case EnemyState::Idle:
            physics.setMoveX(0.0f);
            if (dist <= ai.attackRange) {
                ai.changeState(EnemyState::Attack);
            } else if (dist <= ai.chaseRange) {
                ai.changeState(EnemyState::Chase);
            } else {
                ai.idleTimer += dt;
                if (ai.idleTimer >= ai.idleDuration) {
                    ai.changeState(EnemyState::Patrol);
                }
            }
            break;

        case EnemyState::Patrol:
            if (dist <= ai.attackRange) {
                ai.changeState(EnemyState::Attack);
            } else if (dist <= ai.chaseRange) {
                ai.changeState(EnemyState::Chase);
            } else {
                physics.setMoveX(static_cast<float>(ai.direction));
                if (transform.x <= ai.patrolLeft) {
                    transform.x = ai.patrolLeft;
                    ai.direction = 1;
                    ai.changeState(EnemyState::Idle);
                } else if (transform.x >= ai.patrolRight) {
                    transform.x = ai.patrolRight;
                    ai.direction = -1;
                    ai.changeState(EnemyState::Idle);
                }
            }
            break;

        case EnemyState::Chase:
            if (dist <= ai.attackRange) {
                ai.changeState(EnemyState::Attack);
            } else if (dist >= ai.stopChaseRange) {
                ai.changeState(EnemyState::Patrol);
            } else {
                ai.direction = (playerTransform.x < transform.x) ? -1 : 1;
                physics.setMoveX(static_cast<float>(ai.direction));
            }
            break;

        case EnemyState::Attack:
            physics.setMoveX(0.0f);
            ai.attackTimer += dt;
            if (!ai.damageAppliedThisAttack && ai.attackTimer >= ai.attackDuration * 0.5f) {
                ai.damageAppliedThisAttack = true;
                ai.attackHitThisFrame = true;
            }
            if (ai.attackTimer >= ai.attackDuration) {
                if (dist <= ai.attackRange) ai.changeState(EnemyState::Attack);
                else if (dist <= ai.chaseRange) ai.changeState(EnemyState::Chase);
                else ai.changeState(EnemyState::Patrol);
            }
            break;

        case EnemyState::Hurt:
            physics.setMoveX(0.0f);
            ai.hurtTimer += dt;
            if (ai.hurtTimer >= ai.hurtDuration) {
                if (health.isDead()) ai.changeState(EnemyState::Dead);
                else ai.changeState(EnemyState::Patrol);
            }
            break;

        case EnemyState::Dead:
            physics.setMoveX(0.0f);
            break;
    }
}

void EnemyMovementLogic::updateShooterEnemy(TransformComponent& transform,
                                            PhysicsComponent& physics,
                                            HealthComponent& health,
                                            EnemyAIComponent& ai,
                                            const TransformComponent& playerTransform,
                                            float dt) {
    const float dist = distanceToPlayer(transform, playerTransform, ai);

    switch (ai.state) {
        case EnemyState::Idle:
            physics.setMoveX(0.0f);
            if (dist <= ai.chaseRange) {
                ai.changeState(EnemyState::Attack);
            }
            break;

        case EnemyState::Attack:
            physics.setMoveX(0.0f);
            ai.direction = (playerTransform.x < transform.x) ? -1 : 1;
            ai.attackTimer += dt;
            if (!ai.damageAppliedThisAttack && ai.attackTimer >= 0.5f) {
                ai.damageAppliedThisAttack = true;
                ai.shotThisFrame = true;
            }
            if (ai.attackTimer >= ai.attackDuration) {
                ai.changeState(EnemyState::Idle);
            }
            break;

        case EnemyState::Hurt:
            physics.setMoveX(0.0f);
            ai.hurtTimer += dt;
            if (ai.hurtTimer >= ai.hurtDuration) {
                if (health.isDead()) ai.changeState(EnemyState::Dead);
                else ai.changeState(EnemyState::Idle);
            }
            break;

        case EnemyState::Dead:
            physics.setMoveX(0.0f);
            break;

        default:
            ai.changeState(EnemyState::Idle);
            break;
    }
}

void EnemyMovementLogic::updateFlyingEnemy(TransformComponent& transform,
                                           PhysicsComponent& physics,
                                           HealthComponent& health,
                                           EnemyAIComponent& ai,
                                           const TransformComponent& playerTransform,
                                           float dt) {
    switch (ai.state) {
        case EnemyState::Chase: {
            const float dx = playerTransform.x - transform.x;
            const float dy = playerTransform.y - transform.y;

            ai.direction = (dx < 0.0f) ? -1 : 1;
            transform.x += ai.direction * physics.getSpeed() * dt * 0.8f;

            if (dy < -10.0f) transform.y -= physics.getSpeed() * dt * 0.5f;
            else if (dy > 10.0f) transform.y += physics.getSpeed() * dt * 0.5f;

            if (distanceToPlayer(transform, playerTransform, ai) <= ai.attackRange) {
                ai.changeState(EnemyState::Attack);
            }
            break;
        }

        case EnemyState::Attack:
            ai.direction = (playerTransform.x < transform.x) ? -1 : 1;
            ai.attackTimer += dt;
            if (!ai.damageAppliedThisAttack && ai.attackTimer >= ai.attackDuration * 0.5f) {
                ai.damageAppliedThisAttack = true;
                ai.attackHitThisFrame = true;
            }
            if (ai.attackTimer >= ai.attackDuration) {
                ai.changeState(EnemyState::Chase);
            }
            break;

        case EnemyState::Hurt:
            ai.hurtTimer += dt;
            if (ai.hurtTimer >= ai.hurtDuration) {
                if (health.isDead()) ai.changeState(EnemyState::Dead);
                else ai.changeState(EnemyState::Chase);
            }
            break;

        case EnemyState::Dead:
            break;

        default:
            ai.changeState(EnemyState::Chase);
            break;
    }
}

void EnemyMovementLogic::updateBossEnemy(TransformComponent& transform,
                                         PhysicsComponent& physics,
                                         HealthComponent& health,
                                         EnemyAIComponent& ai,
                                         const TransformComponent& playerTransform,
                                         float dt) {
    switch (ai.state) {
        case EnemyState::Idle:
            physics.setMoveX(0.0f);
            ai.attackTimer += dt;
            if (ai.attackTimer >= 1.0f) {
                ai.attackTimer = 0.0f;
                if (ai.currentBossAttack == BossAttackType::None) {
                    ai.currentBossAttack = (rand() % 2 == 0)
                        ? BossAttackType::BurstShot
                        : BossAttackType::DashAttack;
                    ai.bossAttackPhase = 0;
                    ai.changeState(EnemyState::Attack);
                }
            }
            break;

        case EnemyState::Attack:
            ai.direction = (playerTransform.x < transform.x) ? -1 : 1;

            if (ai.currentBossAttack == BossAttackType::BurstShot) {
                ai.bossAttackTimer += dt;

                if (ai.bossAttackPhase == 0) {
                    physics.setMoveX(0.0f);
                    if (ai.bossAttackTimer >= 0.5f) {
                        ai.bossAttackTimer = 0.0f;
                        ai.bossAttackPhase = 1;
                    }
                } else if (ai.bossAttackPhase == 1) {
                    ai.shotThisFrame = true;
                    ai.bossAttackTimer = 0.0f;
                    ai.bossAttackPhase = 2;
                } else if (ai.bossAttackPhase == 2) {
                    if (ai.bossAttackTimer >= 0.2f) {
                        ai.shotThisFrame = true;
                        ai.bossAttackTimer = 0.0f;
                        ai.bossAttackPhase = 3;
                    }
                } else if (ai.bossAttackPhase == 3) {
                    if (ai.bossAttackTimer >= 0.2f) {
                        ai.shotThisFrame = true;
                        ai.bossAttackTimer = 0.0f;
                        ai.bossAttackPhase = 4;
                    }
                } else if (ai.bossAttackPhase == 4) {
                    if (ai.bossAttackTimer >= 0.4f) {
                        ai.bossAttackTimer = 0.0f;
                        ai.currentBossAttack = BossAttackType::None;
                        ai.changeState(EnemyState::Idle);
                    }
                }
            } else if (ai.currentBossAttack == BossAttackType::DashAttack) {
                ai.bossAttackTimer += dt;

                if (ai.bossAttackPhase == 0) {
                    physics.setMoveX(0.0f);
                    if (ai.bossAttackTimer >= 0.4f) {
                        ai.bossAttackTimer = 0.0f;
                        ai.bossAttackPhase = 1;
                    }
                } else if (ai.bossAttackPhase == 1) {
                    physics.setMoveX(static_cast<float>(ai.direction) * 3.0f);
                    ai.attackHitThisFrame = true;
                    if (ai.bossAttackTimer >= 0.5f) {
                        ai.bossAttackTimer = 0.0f;
                        ai.bossAttackPhase = 2;
                    }
                } else if (ai.bossAttackPhase == 2) {
                    physics.setMoveX(0.0f);
                    if (ai.bossAttackTimer >= 0.5f) {
                        ai.bossAttackTimer = 0.0f;
                        ai.currentBossAttack = BossAttackType::None;
                        ai.changeState(EnemyState::Idle);
                    }
                }
            }
            break;

        case EnemyState::Hurt:
            physics.setMoveX(0.0f);
            ai.hurtTimer += dt;
            if (ai.hurtTimer >= ai.hurtDuration) {
                if (health.isDead()) ai.changeState(EnemyState::Dead);
                else ai.changeState(EnemyState::Idle);
            }
            break;

        case EnemyState::Dead:
            physics.setMoveX(0.0f);
            break;

        default:
            ai.changeState(EnemyState::Idle);
            break;
    }
}