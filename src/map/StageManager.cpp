#include "StageManager.h"

#include <iostream>
#include <utility>

#include "../factories/DoorFactory.h"
#include "../factories/PlayerFactory.h"
#include "../factories/EnemyFactory.h"
#include "../factories/ItemFactory.h"

#include "../components/PlayerTagComponent.h"
#include "../components/TransformComponent.h"

bool StageManager::loadStage(Manager& manager, GameContext& context, const std::string& mapPath) {
    if (!context.map || !context.renderer) {
        return false;
    }

    Map loadedMap;
    if (!loadedMap.load(mapPath.c_str())) {
        std::cout << "Failed to load map: " << mapPath << std::endl;
        return false;
    }

    clearStageEntities(manager);
    context.pendingBulletSpawns.clear();
    *context.map = std::move(loadedMap);

    spawnStageDoors(manager, context);
    spawnOrResetPlayer(manager, context);
    spawnStageEnemies(manager, context);
    spawnStageItems(manager, context);

    return true;
}

void StageManager::clearStageEntities(Manager& manager) {
    for (auto& e : manager.getEntities()) {
        if (!e->hasComponent<PlayerTagComponent>()) {
            e->destroy();
        }
    }
    manager.refresh();
}

void StageManager::spawnStageDoors(Manager& manager, GameContext& context) {
    for (const DoorSpawn& door : context.map->getDoors()) {
        Entity& doorEntity = manager.addEntity();
        DoorFactory::createDoor(
            doorEntity,
            context.renderer,
            door.x,
            door.y,
            door.w,
            door.h,
            door.targetMap
        );
    }
}

void StageManager::spawnOrResetPlayer(Manager& manager, GameContext& context) {
    float startX = 100.0f;
    float startY = 100.0f;

    if (context.map->hasPlayerSpawn()) {
        const SpawnPoint spawn = context.map->getPlayerSpawn();
        startX = spawn.x;
        startY = spawn.y;
    }

    if (!context.playerEntity) {
        context.playerEntity = &manager.addEntity();
        PlayerFactory::createPlayer(*context.playerEntity, context.renderer, startX, startY);
        return;
    }

    auto& transform = context.playerEntity->getComponent<TransformComponent>();
    transform.setPosition(startX, startY);
}

void StageManager::spawnStageEnemies(Manager& manager, GameContext& context) {
    for (const EnemySpawn& spawn : context.map->getEnemySpawns()) {
        Entity& enemy = manager.addEntity();

        const float patrolLeft = spawn.x - 100.0f;
        const float patrolRight = spawn.x + 100.0f;

        EnemyFactory::createEnemy(
            enemy,
            context.renderer,
            spawn.type,
            spawn.x,
            spawn.y,
            patrolLeft,
            patrolRight
        );
    }
}

void StageManager::spawnStageItems(Manager& manager, GameContext& context) {
    for (const ItemSpawn& spawn : context.map->getItemSpawns()) {
        Entity& item = manager.addEntity();
        ItemFactory::createItem(
            item,
            context.renderer,
            spawn.type,
            spawn.x,
            spawn.y
        );
    }
}