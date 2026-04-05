#include "StageManager.h"

#include <iostream>
#include <utility>
#include <filesystem>

#include "../../factories/DoorFactory.h"
#include "../../factories/PlayerFactory.h"
#include "../../factories/EnemyFactory.h"
#include "../../factories/ItemFactory.h"

#include "../../components/identity/PlayerTagComponent.h"
#include "../../components/core/TransformComponent.h"
#include "../../components/gameplay//EnemyAIComponent.h"

bool StageManager::loadStage(Manager& manager,
                             GameContext& context,
                             const std::string& mapPath,
                             const std::string& spawnId) {
    if (!context.map || !context.assetManager) {
        std::cout << "AssetManager::loadStage failed: map or renderer missing.\n";
        return false;
    }

    // Extra debugging messages
    std::cout << "StageManager loading map=[" << mapPath
          << "] spawn=[" << spawnId << "]\n";

    std::cout << "Current working directory: "
              << std::filesystem::current_path() << "\n";

    std::cout << "Exists? "
              << std::filesystem::exists(mapPath) << "\n";

    std::string resolvedPath = mapPath;

    // If the door only gives a filename, build the full path automatically.
    if (resolvedPath.find('/') == std::string::npos &&
        resolvedPath.find('\\') == std::string::npos) {
        resolvedPath = "../assets/maps/" + resolvedPath;
        }

    std::cout << "Resolved map path=[" << resolvedPath << "]\n";

    Map loadedMap;
    if (!loadedMap.load(resolvedPath.c_str())) {
        std::cout << "Failed to load map: [" << resolvedPath << "]\n";
        return false;
    }

    clearStageEntities(manager);
    context.pendingBulletSpawns.clear();
    *context.map = std::move(loadedMap);

    spawnStageDoors(manager, context);
    spawnOrResetPlayer(manager, context, spawnId);
    spawnStageEnemies(manager, context);
    spawnStageItems(manager, context);

    context.stageChangeRequested = false;
    context.requestedStagePath.clear();
    context.requestedSpawnId = "default";
    context.doorTimer = context.doorCooldown;

    std::cout << "Stage loaded successfully: [" << mapPath << "]\n";
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
            *context.assetManager,
            door.x,
            door.y,
            door.w,
            door.h,
            door.targetMap,
            door.targetSpawn
        );
    }
}

void StageManager::spawnOrResetPlayer(Manager& manager,
                                      GameContext& context,
                                      const std::string& spawnId) {
    float startX = 100.0f;
    float startY = 100.0f;

    if (context.map->hasSpawnPoint(spawnId)) {
        const NamedSpawnPoint spawn = context.map->getSpawnPoint(spawnId);
        startX = spawn.x;
        startY = spawn.y;
    } else if (context.map->hasSpawnPoint("default")) {
        const NamedSpawnPoint spawn = context.map->getSpawnPoint("default");
        startX = spawn.x;
        startY = spawn.y;
    } else if (!context.map->getSpawnPoints().empty()) {
        const NamedSpawnPoint& spawn = context.map->getSpawnPoints().front();
        startX = spawn.x;
        startY = spawn.y;
    }

    if (!context.playerEntity) {
        context.playerEntity = &manager.addEntity();
        PlayerFactory::createPlayer(*context.playerEntity, *context.assetManager, startX, startY);
        return;
    }

    auto& transform = context.playerEntity->getComponent<TransformComponent>();
    transform.setPosition(startX, startY);
}

void StageManager::spawnStageEnemies(Manager& manager, GameContext& context) {
    for (const EnemySpawn& spawn : context.map->getEnemySpawns()) {
        Entity& enemy = manager.addEntity();

        EnemyFactory::createEnemy(
            enemy,
            *context.assetManager,
            spawn.type,
            spawn.x,
            spawn.y,
            spawn.patrolLeft,
            spawn.patrolRight
        );

        if (enemy.hasComponent<EnemyAIComponent>()) {
            auto& ai = enemy.getComponent<EnemyAIComponent>();
            ai.direction = spawn.facing;
        }
    }
}

void StageManager::spawnStageItems(Manager& manager, GameContext& context) {
    for (const ItemSpawn& spawn : context.map->getItemSpawns()) {
        Entity& item = manager.addEntity();
        ItemFactory::createItem(
            item,
            *context.assetManager,
            spawn.type,
            spawn.x,
            spawn.y
        );
    }
}