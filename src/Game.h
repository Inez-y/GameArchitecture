#pragma once

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <vector>
#include "Map.h"
#include "Player.h"
#include "Camera.h"
#include "Enemy.h"
#include "Bullet.h"
#include "Item.h"

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
    bool loadStage(const char* mathPath);

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* tilesetTexture;
    bool isRunning;

    Map map;
    Player player;
    Camera camera;
    std::vector<Enemy> enemies; // multiple enemies
    std::vector<Bullet> bullets;
    float shootCooldown;
    float shootTimer;
    std::vector<Item> items;
    int coinCount;

    float doorTimer;
    float doorCooldown;
    SDL_Texture* doorTexture;

    // Previous tick
    Uint64 lastCounter;
};