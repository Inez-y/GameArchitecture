/*
 * Draw gameplay entities
 */
#ifndef LASTCARRIAGE_UISYSTEM_H
#define LASTCARRIAGE_UISYSTEM_H

#pragma once

#include "../ecs/Manager.h"
#include "../GameContext.h"

class RenderSystem {
public:
    void render(Manager& manager, const GameContext& context) const;

private:
    void renderDoors(Manager& manager, const GameContext& context) const;
    void renderItems(Manager& manager, const GameContext& context) const;
    void renderEnemies(Manager& manager, const GameContext& context) const;
    void renderBullets(Manager& manager, const GameContext& context) const;
    void renderHUD(const GameContext& context) const;
};

#endif