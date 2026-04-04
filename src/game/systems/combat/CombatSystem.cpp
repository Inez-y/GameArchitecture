#include "CombatSystem.h"

#include <iostream>

#include "../../factories/BulletFactory.h"

#include "../../components/core/TransformComponent.h"
#include "../../components/identity/BulletTagComponent.h"
#include "../../components/gameplay/BulletComponent.h"
#include "../../components/identity/EnemyTagComponent.h"
#include "../../components/gameplay/EnemyAIComponent.h"
#include "../../components/core/HealthComponent.h"

bool CombatSystem::overlaps(const SDL_FRect& a, const SDL_FRect& b) {
    return a.x < b.x + b.w &&
           a.x + a.w > b.x &&
           a.y < b.y + b.h &&
           a.y + a.h > b.y;
}

void CombatSystem::update(Manager& manager, GameContext& context, float dt) {
    if (!context.playerEntity || !context.map) {
        return;
    }

    if (context.shootTimer > 0.0f) {
        context.shootTimer -= dt;
        if (context.shootTimer < 0.0f) {
            context.shootTimer = 0.0f;
        }
    }

    queueEnemyBulletSpawns(manager, context);
    spawnQueuedBullets(manager, context);
    updateBullets(manager, context, dt);
    handleBulletCollisions(manager, context);
    handleEnemyContactDamage(manager, context);
}

void CombatSystem::queueEnemyBulletSpawns(Manager& manager, GameContext& context) {
    for (auto& e : manager.getEntities()) {
        if (!e->hasComponent<EnemyTagComponent>()) {
            continue;
        }

        auto& transform = e->getComponent<TransformComponent>();
        auto& ai = e->getComponent<EnemyAIComponent>();
        auto& health = e->getComponent<HealthComponent>();

        if (health.isDead()) {
            continue;
        }

        if (ai.shotThisFrame) {
            context.pendingBulletSpawns.push_back({
                BulletOwner::Enemy,
                transform.x + transform.w * 0.5f,
                transform.y + transform.h * 0.5f,
                ai.direction
            });
            ai.shotThisFrame = false;
        }
    }
}

void CombatSystem::spawnQueuedBullets(Manager& manager, GameContext& context) {
    for (const BulletSpawnRequest& req : context.pendingBulletSpawns) {
        Entity& bullet = manager.addEntity();
        BulletFactory::createBullet(
            bullet,
            context.renderer,
            req.owner,
            req.x,
            req.y,
            req.dir
        );
    }

    context.pendingBulletSpawns.clear();
}

void CombatSystem::updateBullets(Manager& manager, GameContext& context, float dt) {
    const float mapPixelWidth = static_cast<float>(context.map->getWidth() * context.map->getTileWidth());

    for (auto& e : manager.getEntities()) {
        if (!e->hasComponent<BulletTagComponent>()) {
            continue;
        }

        auto& transform = e->getComponent<TransformComponent>();
        auto& bullet = e->getComponent<BulletComponent>();

        if (!bullet.isActive()) {
            continue;
        }

        transform.x += bullet.getDirection() * bullet.getSpeed() * dt;

        if (transform.x < 0.0f || transform.x > mapPixelWidth) {
            bullet.deactivate();
            e->destroy();
        }
    }
}

void CombatSystem::handleBulletCollisions(Manager& manager, GameContext& context) {
    auto& playerTransform = context.playerEntity->getComponent<TransformComponent>();
    auto& playerHealth = context.playerEntity->getComponent<HealthComponent>();
    const SDL_FRect playerBounds = playerTransform.getRect();

    for (auto& bulletEntity : manager.getEntities()) {
        if (!bulletEntity->hasComponent<BulletTagComponent>()) {
            continue;
        }

        auto& bulletTransform = bulletEntity->getComponent<TransformComponent>();
        auto& bullet = bulletEntity->getComponent<BulletComponent>();

        if (!bullet.isActive()) {
            continue;
        }

        const SDL_FRect bulletBounds = bulletTransform.getRect();

        if (bullet.getOwner() == BulletOwner::Player) {
            for (auto& enemyEntity : manager.getEntities()) {
                if (!enemyEntity->hasComponent<EnemyTagComponent>()) {
                    continue;
                }

                auto& enemyTransform = enemyEntity->getComponent<TransformComponent>();
                auto& enemyHealth = enemyEntity->getComponent<HealthComponent>();
                auto& enemyAI = enemyEntity->getComponent<EnemyAIComponent>();

                if (enemyHealth.isDead()) {
                    continue;
                }

                if (overlaps(bulletBounds, enemyTransform.getRect())) {
                    enemyHealth.takeDamage(bullet.getDamage());

                    if (enemyHealth.isDead()) {
                        enemyAI.changeState(EnemyState::Dead);
                        enemyEntity->destroy();
                        std::cout << "Enemy killed and removed.\n";
                    } else {
                        enemyAI.changeState(EnemyState::Hurt);
                    }

                    bullet.deactivate();
                    bulletEntity->destroy();
                    break;
                }
            }
        } else {
            if (overlaps(playerBounds, bulletBounds)) {
                playerHealth.takeDamage(bullet.getDamage());
                bullet.deactivate();
                bulletEntity->destroy();
                std::cout << "Player hit by enemy bullet. HP: " << playerHealth.getHP() << std::endl;
            }
        }
    }
}

void CombatSystem::handleEnemyContactDamage(Manager& manager, GameContext& context) {
    auto& playerTransform = context.playerEntity->getComponent<TransformComponent>();
    auto& playerHealth = context.playerEntity->getComponent<HealthComponent>();
    const SDL_FRect playerBounds = playerTransform.getRect();

    for (auto& e : manager.getEntities()) {
        if (!e->hasComponent<EnemyTagComponent>()) {
            continue;
        }

        auto& transform = e->getComponent<TransformComponent>();
        auto& ai = e->getComponent<EnemyAIComponent>();
        auto& health = e->getComponent<HealthComponent>();

        if (health.isDead() || !ai.attackHitThisFrame) {
            continue;
        }

        if (overlaps(playerBounds, transform.getRect())) {
            playerHealth.takeDamage(1);
            std::cout << "Player took 1 damage. HP: " << playerHealth.getHP() << std::endl;
        }

        ai.resetAttackHit();
    }
}