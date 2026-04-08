/**
* Shared state container
 */
#pragma once

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <vector>
#include <string>

#include "engine/core/Camera.h"
#include "../src/game/map/runtime/Map.h"
#include "../src/game/components/gameplay/BulletComponent.h"

class Entity;
class AssetManager;

struct BulletSpawnRequest {
    BulletOwner owner;
    float x;
    float y;
    int dir;
};

struct GameContext {
    SDL_Renderer* renderer = nullptr;
    AssetManager* assetManager = nullptr;

    SDL_Texture* tilesetTexture = nullptr;
    SDL_Texture* backgroundTexture = nullptr;   // NEW
    TTF_Font* uiFont = nullptr;

    Camera* camera = nullptr;
    Map* map = nullptr;
    Entity* playerEntity = nullptr;

    float shootCooldown = 0.2f;
    float shootTimer = 0.0f;

    float doorCooldown = 0.5f;
    float doorTimer = 0.0f;

    int coinCount = 0;

    bool stageChangeRequested = false;
    std::string requestedStagePath;
    std::string requestedSpawnId = "default";

    std::vector<BulletSpawnRequest> pendingBulletSpawns;
};