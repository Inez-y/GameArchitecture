// Shared state container
#ifndef LASTCARRIAGE_GAMECONTEXT_H
#define LASTCARRIAGE_GAMECONTEXT_H

#pragma once

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <vector>
#include <string>

#include "Camera.h"
#include "map/Map.h"
#include "components/BulletComponent.h"

class Entity;

struct BulletSpawnRequest {
    BulletOwner owner;
    float x;
    float y;
    int dir;
};

struct GameContext {
    TTF_Font* uiFont = nullptr;

    SDL_Renderer* renderer = nullptr;
    SDL_Texture* tilesetTexture = nullptr;

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

    std::vector<BulletSpawnRequest> pendingBulletSpawns;
};
#endif