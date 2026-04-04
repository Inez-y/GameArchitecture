/**
 * Tunrs map data into entities
 * Gameplay spawning/orcheestration
 */
#ifndef LASTCARRIAGE_STAGEMANAGER_H
#define LASTCARRIAGE_STAGEMANAGER_H

#pragma once

#include <string>

#include "../../../engine/ecs/Manager.h"
#include "../../../GameContext.h"
#include "Map.h"

class StageManager {
public:
    bool loadStage(Manager& manager,
                   GameContext& context,
                   const std::string& mapPath,
                   const std::string& spawnId = "default");

private:
    void clearStageEntities(Manager& manager);
    void spawnStageDoors(Manager& manager, GameContext& context);
    void spawnOrResetPlayer(Manager& manager, GameContext& context, const std::string& spawnId);
    void spawnStageEnemies(Manager& manager, GameContext& context);
    void spawnStageItems(Manager& manager, GameContext& context);
};

#endif