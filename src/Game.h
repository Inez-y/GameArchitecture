#ifndef LASTCARRIAGE_GAME_H
#define LASTCARRIAGE_GAME_H

#pragma once

#include <memory>

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include "engine/resources/AssetManager.h"
#include "game/scenes/IScene.h"

class Game {
public:
    Game();
    ~Game();

    bool init(const char* title, int width, int height, bool fullscreen);
    void run();
    void clean();

    bool running() const { return isRunning; }

private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    bool isRunning = false;

    int screenWidth = 1280;
    int screenHeight = 720;

    AssetManager assetManager;
    std::unique_ptr<IScene> currentScene;
};

#endif // LASTCARRIAGE_GAME_H