#include "Game.h"

#include <iostream>

#include "components/PlayerTagComponent.h"
#include "components/TransformComponent.h"
#include "components/PhysicsComponent.h"
#include "components/WeaponComponent.h"
#include "components/HealthComponent.h"

Game::Game() = default;

Game::~Game() {
    clean();
}

bool Game::init(const char* title, int width, int height, bool fullscreen) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cout << "SDL init failed: " << SDL_GetError() << std::endl;
        return false;
    }

    Uint32 windowFlags = fullscreen ? SDL_WINDOW_FULLSCREEN : 0;

    window = SDL_CreateWindow(title, width, height, windowFlags);
    if (!window) {
        std::cout << "Window creation failed: " << SDL_GetError() << std::endl;
        return false;
    }

    if (!TTF_Init()) {
        std::cout << "TTF init failed: " << SDL_GetError() << std::endl;
        return false;
    }

    context.uiFont = TTF_OpenFont("../assets/Fonts/PixelifySans_Font.ttf", 24);
    if (!context.uiFont) {
        std::cout << "Failed to load UI font: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::cout << "Renderer creation failed: " << SDL_GetError() << std::endl;
        return false;
    }

    tilesetTexture = IMG_LoadTexture(renderer, "../assets/Maps/tileset.png");
    if (!tilesetTexture) {
        std::cout << "Failed to load tileset texture: " << SDL_GetError() << std::endl;
        return false;
    }

    camera.w = static_cast<float>(width);
    camera.h = static_cast<float>(height);

    context.renderer = renderer;
    context.tilesetTexture = tilesetTexture;
    context.camera = &camera;
    context.map = &map;
    context.playerEntity = nullptr;
    context.stageChangeRequested = false;
    context.requestedStagePath.clear();
    context.pendingBulletSpawns.clear();
    context.coinCount = 0;
    context.shootCooldown = 0.2f;
    context.shootTimer = 0.0f;
    context.doorCooldown = 0.5f;
    context.doorTimer = 0.0f;

    if (!stageManager.loadStage(manager, context, "../assets/Maps/map1.tmx")) {
        std::cout << "Failed to load initial stage." << std::endl;
        return false;
    }

    isRunning = true;
    return true;
}

void Game::run() {
    Uint64 lastCounter = SDL_GetPerformanceCounter();

    while (isRunning) {
        Uint64 currentCounter = SDL_GetPerformanceCounter();
        float dt = static_cast<float>(currentCounter - lastCounter) /
                   static_cast<float>(SDL_GetPerformanceFrequency());
        lastCounter = currentCounter;

        handleEvents();
        update(dt);
        render();
    }
}

void Game::handleEvents() {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            isRunning = false;
        }

        if (!context.playerEntity) {
            continue;
        }

        auto& weapon = context.playerEntity->getComponent<WeaponComponent>();
        auto& transform = context.playerEntity->getComponent<TransformComponent>();

        if (event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat) {
            if (event.key.scancode == SDL_SCANCODE_R) {
                weapon.startReload();
            }

            if (event.key.scancode == SDL_SCANCODE_SPACE &&
                context.shootTimer <= 0.0f &&
                weapon.canShoot()) {

                context.pendingBulletSpawns.push_back({
                    BulletOwner::Player,
                    transform.x + transform.w * 0.5f,
                    transform.y + transform.h * 0.5f,
                    weapon.getFacingDirection()
                });

                weapon.consumeAmmo();
                context.shootTimer = context.shootCooldown;
                }
        }
    }

    if (context.playerEntity && context.playerEntity->hasComponent<InputComponent>()) {
        const bool* keyStates = SDL_GetKeyboardState(nullptr);
        auto& input = context.playerEntity->getComponent<InputComponent>();
        input.handleInput(keyStates);
    }
}

void Game::update(float dt) {
    if (!context.playerEntity) {
        return;
    }

    movementSystem.update(manager, context, dt);
    combatSystem.update(manager, context, dt);
    itemSystem.update(manager, context);

    manager.refresh();

    doorSystem.update(manager, context, dt);

    if (context.stageChangeRequested) {
        const std::string nextStage = context.requestedStagePath;

        context.stageChangeRequested = false;
        context.requestedStagePath.clear();

        if (!nextStage.empty()) {
            stageManager.loadStage(manager, context, nextStage);
        }
    }

    manager.refresh();
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_FRect cameraRect{camera.x, camera.y, camera.w, camera.h};
    map.render(renderer, tilesetTexture, cameraRect);

    renderSystem.render(manager, context);

    SDL_RenderPresent(renderer);
}

void Game::clean() {
    if (context.uiFont) {
        TTF_CloseFont(context.uiFont);
        context.uiFont = nullptr;
    }

    if (tilesetTexture) {
        SDL_DestroyTexture(tilesetTexture);
        tilesetTexture = nullptr;
    }

    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    TTF_Quit();
    SDL_Quit();

    isRunning = false;
}