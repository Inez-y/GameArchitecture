#include "Game.h"

#include <iostream>

#include "game/scenes/PlayScene.h"
#include "game/systems/audio/BgmPlayer.h"

Game::Game() = default;

Game::~Game() {
    clean();
}

bool Game::init(const char* title, int width, int height, bool fullscreen) {
    screenWidth = width;
    screenHeight = height;

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cout << "SDL init failed: " << SDL_GetError() << std::endl;
        return false;
    }

    if (!TTF_Init()) {
        std::cout << "TTF init failed: " << SDL_GetError() << std::endl;
        return false;
    }

    Uint32 windowFlags = fullscreen ? SDL_WINDOW_FULLSCREEN : 0;
    window = SDL_CreateWindow(title, width, height, windowFlags);
    if (!window) {
        std::cout << "Window creation failed: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::cout << "Renderer creation failed: " << SDL_GetError() << std::endl;
        return false;
    }

    assetManager.setRenderer(renderer);

    currentScene = std::make_unique<PlayScene>(renderer, assetManager, width, height);
    if (!currentScene->init()) {
        std::cout << "Failed to initialize PlayScene.\n";
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

        if (currentScene) {
            if (!currentScene->handleEvents()) {
                isRunning = false;
                break;
            }
            currentScene->update(dt);
            currentScene->render();
        }
    }
}

void Game::clean() {
    if (currentScene) {
        currentScene->clean();
        currentScene.reset();
    }

    assetManager.clear();

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