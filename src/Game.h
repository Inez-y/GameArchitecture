#pragma once

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "Map.h"
#include "Player.h"
#include "Camera.h"

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

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* tilesetTexture;
    bool isRunning;

    Map map;
    Player player;
    Camera camera;

    // Previous tick
    Uint64 lastCounter;
};