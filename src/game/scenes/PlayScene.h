/**
 * Own Manager/Map/Camera/GameContext/StateManager/gameplay systems
 *  Load first stage
 *  Handle gameplay update/render flow
 *
 *  Where the world state moves
 */
#ifndef LASTCARRIAGE_PLAYSCENE_H
#define LASTCARRIAGE_PLAYSCENE_H

#pragma once

#include "IScene.h"

#include "../../GameContext.h"
#include "../../engine/ecs/Manager.h"
#include "../../engine/core/Camera.h"
#include "../../engine/resources/AssetManager.h"

#include "../map/runtime/Map.h"
#include "../map/runtime/StageManager.h"

#include "../systems/interaction/DoorSystem.h"
#include "../systems/interaction/ItemSystem.h"
#include "../systems/combat/CombatSystem.h"
#include "../systems/rendering/RenderSystem.h"
#include "../systems/movement/MovementSystem.h"

#include "../components/gameplay/WeaponComponent.h"
#include "../components/core/TransformComponent.h"
#include "../components/core/InputComponent.h"
#include "../data/AssetPaths.h"

class PlayScene : public IScene {
public:
    PlayScene(SDL_Renderer* renderer, AssetManager& assetManager, int screenWidth, int screenHeight);

    bool init() override;
    bool handleEvents() override;
    void update(float dt) override;
    void render() override;
    void clean() override;

private:
    SDL_Renderer* renderer = nullptr;
    AssetManager& assetManager;

    Manager manager;
    Map map;
    Camera camera{0.0f, 0.0f, 1280.0f, 720.0f};
    GameContext context;

    SDL_Texture* tilesetTexture = nullptr;

    DoorSystem doorSystem;
    ItemSystem itemSystem;
    CombatSystem combatSystem;
    RenderSystem renderSystem;
    MovementSystem movementSystem;
    StageManager stageManager;
};

#endif // LASTCARRIAGE_PLAYSCENE_H