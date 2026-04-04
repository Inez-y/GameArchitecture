#include "MovementSystem.h"

#include <algorithm>

#include "../../components/core/TransformComponent.h"
#include "../../components/core/PhysicsComponent.h"
#include "../../components/gameplay/WeaponComponent.h"
#include "../../components/identity/EnemyTagComponent.h"
#include "../../components/gameplay//EnemyAIComponent.h"
#include "../../components/core/HealthComponent.h"

void MovementSystem::update(Manager& manager, GameContext& context, float dt) {
    if (!context.playerEntity || !context.map) {
        return;
    }

    updatePlayer(context, dt);
    updateEnemies(manager, context, dt);
    updateCamera(context);
}

void MovementSystem::updatePlayer(GameContext& context, float dt) {
    auto& playerPhysics = context.playerEntity->getComponent<PhysicsComponent>();
    auto& playerWeapon = context.playerEntity->getComponent<WeaponComponent>();

    playerPhysics.update(dt, *context.map);
    playerWeapon.update(dt);
}

void MovementSystem::updateEnemies(Manager& manager, GameContext& context, float dt) {
    auto& playerTransform = context.playerEntity->getComponent<TransformComponent>();

    for (auto& e : manager.getEntities()) {
        if (!e->hasComponent<EnemyTagComponent>()) {
            continue;
        }

        auto& transform = e->getComponent<TransformComponent>();
        auto& physics = e->getComponent<PhysicsComponent>();
        auto& health = e->getComponent<HealthComponent>();
        auto& ai = e->getComponent<EnemyAIComponent>();

        ai.beginFrame();

        if (health.isDead()) {
            ai.changeState(EnemyState::Dead);
        }

        switch (ai.type) {
            case EnemyType::Patrol:
                switch (ai.state) {
                    case EnemyState::Idle:
                        physics.setMoveX(0.0f);
                        if (ai.distanceTo(transform.x, transform.y, playerTransform.x, playerTransform.y) <= ai.attackRange) {
                            ai.changeState(EnemyState::Attack);
                        } else if (ai.distanceTo(transform.x, transform.y, playerTransform.x, playerTransform.y) <= ai.chaseRange) {
                            ai.changeState(EnemyState::Chase);
                        } else {
                            ai.idleTimer += dt;
                            if (ai.idleTimer >= ai.idleDuration) {
                                ai.changeState(EnemyState::Patrol);
                            }
                        }
                        break;

                    case EnemyState::Patrol:
                        if (ai.distanceTo(transform.x, transform.y, playerTransform.x, playerTransform.y) <= ai.attackRange) {
                            ai.changeState(EnemyState::Attack);
                        } else if (ai.distanceTo(transform.x, transform.y, playerTransform.x, playerTransform.y) <= ai.chaseRange) {
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
                        if (ai.distanceTo(transform.x, transform.y, playerTransform.x, playerTransform.y) <= ai.attackRange) {
                            ai.changeState(EnemyState::Attack);
                        } else if (ai.distanceTo(transform.x, transform.y, playerTransform.x, playerTransform.y) >= ai.stopChaseRange) {
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
                            const float dist = ai.distanceTo(transform.x, transform.y, playerTransform.x, playerTransform.y);
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
                break;

            case EnemyType::Shooter:
                switch (ai.state) {
                    case EnemyState::Idle:
                        physics.setMoveX(0.0f);
                        if (ai.distanceTo(transform.x, transform.y, playerTransform.x, playerTransform.y) <= ai.chaseRange) {
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
                        if (ai.attackTimer >= 1.0f) {
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
                break;

            case EnemyType::Flying:
                switch (ai.state) {
                    case EnemyState::Chase: {
                        const float dx = playerTransform.x - transform.x;
                        const float dy = playerTransform.y - transform.y;
                        ai.direction = (dx < 0.0f) ? -1 : 1;
                        transform.x += ai.direction * physics.getSpeed() * dt * 0.8f;
                        if (dy < -10.0f) transform.y -= physics.getSpeed() * dt * 0.5f;
                        else if (dy > 10.0f) transform.y += physics.getSpeed() * dt * 0.5f;

                        if (ai.distanceTo(transform.x, transform.y, playerTransform.x, playerTransform.y) <= ai.attackRange) {
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
                break;

            case EnemyType::Boss:
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
                        if (ai.currentBossAttack == BossAttackType::BurstShot) {
                            ai.bossAttackTimer += dt;
                            ai.direction = (playerTransform.x < transform.x) ? -1 : 1;

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
                            ai.direction = (playerTransform.x < transform.x) ? -1 : 1;

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
                break;
        }

        physics.update(dt, *context.map);

        if (health.isDead()) {
            ai.changeState(EnemyState::Dead);
        }
    }
}

void MovementSystem::updateCamera(GameContext& context) {
    if (!context.playerEntity || !context.map || !context.camera) {
        return;
    }

    auto& playerTransform = context.playerEntity->getComponent<TransformComponent>();

    const float mapPixelWidth = static_cast<float>(context.map->getWidth() * context.map->getTileWidth());
    const float mapPixelHeight = static_cast<float>(context.map->getHeight() * context.map->getTileHeight());

    context.camera->x = playerTransform.x + playerTransform.w * 0.5f - context.camera->w * 0.5f;
    context.camera->y = playerTransform.y + playerTransform.h * 0.5f - context.camera->h * 0.5f;

    if (mapPixelWidth <= context.camera->w) {
        context.camera->x = 0.0f;
    } else {
        context.camera->x = std::clamp(context.camera->x, 0.0f, mapPixelWidth - context.camera->w);
    }

    if (mapPixelHeight <= context.camera->h) {
        context.camera->y = 0.0f;
    } else {
        context.camera->y = std::clamp(context.camera->y, 0.0f, mapPixelHeight - context.camera->h);
    }
}