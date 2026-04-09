#include "PlayScene.h"

#include <iostream>

PlayScene::PlayScene(SDL_Renderer* renderer, AssetManager& assetManager, int screenWidth, int screenHeight)
    : renderer(renderer), assetManager(assetManager) {
    camera.w = static_cast<float>(screenWidth);
    camera.h = static_cast<float>(screenHeight);
}

bool PlayScene::init() {
    tilesetTexture = assetManager.getTexture(AssetPaths::TILESET);
    if (!tilesetTexture) {
        std::cout << "PlayScene: failed to load tileset texture.\n";
        return false;
    }

    context.renderer = renderer;
    context.assetManager = &assetManager;
    context.tilesetTexture = tilesetTexture;
    context.backgroundTexture = assetManager.getTexture(AssetPaths::MAP_1_BACKGROUND);
    context.uiFont = assetManager.getFont(AssetPaths::UI_FONT, 24);
    context.camera = &camera;
    context.map = &map;
    context.playerEntity = nullptr;

    context.shootCooldown = 0.2f;
    context.shootTimer = 0.0f;
    context.doorCooldown = 0.5f;
    context.doorTimer = 0.0f;
    context.coinCount = 0;
    context.stageChangeRequested = false;
    context.requestedStagePath.clear();
    context.requestedSpawnId = "default";
    context.pendingBulletSpawns.clear();

    if (!context.uiFont) {
        std::cout << "PlayScene: failed to load UI font.\n";
        return false;
    }

    if (!context.backgroundTexture) {
        std::cout << "PlayScene: failed to load map background texture.\n";
        return false;
    }

    if (!stageManager.loadStage(manager, context, AssetPaths::MAP_1, "default")) {
        std::cout << "PlayScene: failed to load initial stage.\n";
        return false;
    }

    if (!audioSystem.init()) {
        return false;
    }

    audioSystem.playBgm(AssetPaths::BGM, 0.5f);


    return true;
}

bool PlayScene::handleEvents() {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            return false;
        }

        if (!context.playerEntity) {
            continue;
        }

        if (!context.playerEntity->hasComponent<WeaponComponent>() ||
            !context.playerEntity->hasComponent<TransformComponent>()) {
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

    return true;
}

void PlayScene::update(float dt) {
    if (!context.playerEntity) {
        return;
    }

    movementSystem.update(manager, context, dt);
    combatSystem.update(manager, context, dt);
    itemSystem.update(manager, context);
    animationSystem.update(manager, context, dt);

    manager.refresh();

    doorSystem.update(manager, context, dt);

    if (context.stageChangeRequested) {
        const std::string nextStage = context.requestedStagePath;
        const std::string nextSpawn = context.requestedSpawnId.empty()
                                      ? "default"
                                      : context.requestedSpawnId;

        context.stageChangeRequested = false;
        context.requestedStagePath.clear();
        context.requestedSpawnId = "default";

        if (!nextStage.empty()) {
            if (nextStage == AssetPaths::MAP_1_BOSS) {
                context.backgroundTexture = assetManager.getTexture(AssetPaths::MAP_1_BOSS_BACKGROUND);
            } else {
                context.backgroundTexture = assetManager.getTexture(AssetPaths::MAP_1_BACKGROUND);
            }

            if (!context.backgroundTexture) {
                std::cout << "Failed to load background for stage=[" << nextStage << "]\n";
            }

            if (!stageManager.loadStage(manager, context, nextStage, nextSpawn)) {
                std::cout << "Stage transition failed for map=[" << nextStage
                          << "] spawn=[" << nextSpawn << "]\n";
            }
        }
    }

    audioSystem.update(dt);

    manager.refresh();
}

void PlayScene::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Draw screenshot background first
    if (context.backgroundTexture) {
        SDL_FRect bgRect{0.0f, 0.0f, camera.w, camera.h};
        SDL_RenderTexture(renderer, context.backgroundTexture, nullptr, &bgRect);
    }

    // Then draw tile map and entities on top
    SDL_FRect cameraRect{camera.x, camera.y, camera.w, camera.h};

    // Skip rendering the old tileset
    // map.render(renderer, tilesetTexture, cameraRect);

    renderSystem.render(manager, context);
    SDL_RenderPresent(renderer);
}

void PlayScene::clean() {
    context.playerEntity = nullptr;
    context.map = nullptr;
    context.camera = nullptr;
    context.assetManager = nullptr;
    context.renderer = nullptr;
    context.tilesetTexture = nullptr;
    context.backgroundTexture = nullptr;
    context.uiFont = nullptr;
    tilesetTexture = nullptr;
    audioSystem.clean();
}
