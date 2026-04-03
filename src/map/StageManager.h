/*
 * Load map, spawn doors/items/enemies/player
 */
#ifndef LASTCARRIAGE_STORAGEMANAGER_H
#define LASTCARRIAGE_STORAGEMANAGER_H

#pragma once

#include <string>

#include "../ecs/Manager.h"
#include "../GameContext.h"
#include "Map.h"

class StageManager {
public:
    bool loadStage(Manager& manager, GameContext& context, const std::string& mapPath);

private:
    void clearStageEntities(Manager& manager);
    void spawnStageDoors(Manager& manager, GameContext& context);
    void spawnOrResetPlayer(Manager& manager, GameContext& context);
    void spawnStageEnemies(Manager& manager, GameContext& context);
    void spawnStageItems(Manager& manager, GameContext& context);
};

#endif //LASTCARRIAGE_STORAGEMANAGER_H