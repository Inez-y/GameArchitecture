#ifndef LASTCARRIAGE_GAME_H
#define LASTCARRIAGE_GAME_H

#pragma once

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

#include "map/Map.h"

#include "ecs/Manager.h"

#include "factories/PlayerFactory.h"
#include "factories/EnemyFactory.h"
#include "factories/ItemFactory.h"
#include "factories/BulletFactory.h"
#include "factories/DoorFactory.h"

#include "components/PlayerTagComponent.h"
#include "components/EnemyTagComponent.h"
#include "components/TransformComponent.h"
#include "components/SpriteComponent.h"
#include "components/PhysicsComponent.h"
#include "components/HealthComponent.h"
#include "components/WeaponComponent.h"
#include "components/InputComponent.h"
#include "components/EnemyAIComponent.h"
#include "components/ItemComponent.h"
#include "components/ItemTagComponent.h"
#include "components/BulletTagComponent.h"
#include "components/BulletComponent.h"
#include "components/DoorTagComponent.h"
#include "components/DoorComponent.h"

struct Camera {
    float x = 0.0f;
    float y = 0.0f;
    float w = 0.0f;
    float h = 0.0f;
};

void applyItemEffect(ItemType type);
struct BulletSpawnRequest {
    BulletOwner owner;
    float x;
    float y;
    int dir;
};

class Game {
public:
    Game();
    ~Game();

    bool init(const char* title, int width, int height);
    void run();
    void clean();

private:
    void handleEvents();
    void update();
    void render();

    bool loadStage(const std::string& mapPath);

    bool updateHPText();
    bool updateAmmoText();

    void applyItemEffect(ItemType type);

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* tilesetTexture;
    SDL_Texture* doorTexture;

    TTF_Font* uiFont;
    SDL_Texture* hpTextTexture;
    SDL_FRect hpTextRect;

    SDL_Texture* ammoTextTexture;
    SDL_FRect ammoTextRect;

    bool isRunning;
    Uint64 lastCounter;

    float shootCooldown;
    float shootTimer;

    float doorTimer;
    float doorCooldown;

    int coinCount;

    Camera camera;
    Map map;

    // ECS
    Manager manager;
    Entity* playerEntity;

    std::vector<BulletSpawnRequest> pendingBulletSpawns;

};

#endif // LASTCARRIAGE_GAME_H
