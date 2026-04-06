/**
 * Validate context, update player movements, loop through enemies, call EnemyMovementLogic::updateEnemy, update camera
 *
 * Helpers do not need to be in the header
 * - the header stays small
 * - other files only see the public system interface
 * - enemy AI implementation details stay private
 * - isContextValid(...), isEnemyMovable(...), distanceToPlayer(...), updatePatrolEnemy(...), updateShooterEnemy(...) updateFlyingEnemy(...), updateBossEnemy(...), updateEnemyByType(...),
 */
#include "MovementSystem.h"

#include <algorithm>

#include "../../components/core/TransformComponent.h"
#include "../../components/core/PhysicsComponent.h"
#include "../../components/core/HealthComponent.h"
#include "../../components/gameplay/WeaponComponent.h"
#include "../../components/identity/EnemyTagComponent.h"
#include "../../components/gameplay/EnemyAIComponent.h"
#include "EnemyMovementLogic.h"

static bool isContextValid(GameContext& context) {
    return context.playerEntity && context.map;
}

static bool isEnemyMovable(Entity& e) {
    return e.hasComponent<EnemyTagComponent>() &&
           e.hasComponent<TransformComponent>() &&
           e.hasComponent<PhysicsComponent>() &&
           e.hasComponent<HealthComponent>() &&
           e.hasComponent<EnemyAIComponent>();
}

void MovementSystem::update(Manager& manager, GameContext& context, float dt) {
    if (!isContextValid(context)) {
        return;
    }

    updatePlayer(context, dt);
    updateEnemies(manager, context, dt);
    updateCamera(context);
}

void MovementSystem::updatePlayer(GameContext& context, float dt) {
    if (!context.playerEntity ||
        !context.map ||
        !context.playerEntity->hasComponent<PhysicsComponent>() ||
        !context.playerEntity->hasComponent<WeaponComponent>()) {
        return;
    }

    auto& playerPhysics = context.playerEntity->getComponent<PhysicsComponent>();
    auto& playerWeapon = context.playerEntity->getComponent<WeaponComponent>();

    playerPhysics.update(dt, *context.map);
    playerWeapon.update(dt);
}

void MovementSystem::updateEnemies(Manager& manager, GameContext& context, float dt) {
    if (!context.playerEntity->hasComponent<TransformComponent>()) {
        return;
    }

    auto& playerTransform = context.playerEntity->getComponent<TransformComponent>();

    for (auto& e : manager.getEntities()) {
        if (!isEnemyMovable(*e)) {
            continue;
        }

        auto& transform = e->getComponent<TransformComponent>();
        auto& physics = e->getComponent<PhysicsComponent>();
        auto& health = e->getComponent<HealthComponent>();
        auto& ai = e->getComponent<EnemyAIComponent>();

        ai.beginFrame();
        EnemyMovementLogic::updateEnemy(transform, physics, health, ai, playerTransform, dt);
        physics.update(dt, *context.map);
    }
}

void MovementSystem::updateCamera(GameContext& context) {
    if (!context.playerEntity ||
        !context.map ||
        !context.camera ||
        !context.playerEntity->hasComponent<TransformComponent>()) {
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