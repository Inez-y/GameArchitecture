#ifndef LASTCARRIAGE_GAME_H
#define LASTCARRIAGE_GAME_H

#pragma once

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "ecs/Manager.h"

#include "Camera.h"
#include "GameContext.h"

#include "systems/DoorSystem.h"
#include "systems/ItemSystem.h"
#include "systems/CombatSystem.h"
#include "systems/RenderSystem.h"
#include "systems/MovementSystem.h"

#include "components/InputComponent.h"

#include "map/Map.h"
#include "map/StageManager.h"

class Game {
public:
    Game();
    ~Game();

    bool init(const char* title, int width, int height, bool fullscreen);
    void run();
    void handleEvents();
    void update(float dt);
    void render();
    void clean();

    bool running() const { return isRunning; }

private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* tilesetTexture = nullptr;

    bool isRunning = false;

    Manager manager;
    Map map;
    Camera camera{0.0f, 0.0f, 1280.0f, 720.0f};

    GameContext context;

    DoorSystem doorSystem;
    ItemSystem itemSystem;
    CombatSystem combatSystem;
    RenderSystem renderSystem;
    MovementSystem movementSystem;
    StageManager stageManager;
};

#endif // LASTCARRIAGE_GAME_H