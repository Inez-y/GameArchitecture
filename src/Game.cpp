#include "Game.h"
#include <iostream>

Game::Game()
    : window(nullptr),
      renderer(nullptr),
      tilesetTexture(nullptr),
      isRunning(false) {
}

Game::~Game() {
}

bool Game::init(const char* title, int width, int height) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cout << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow(title, width, height, 0);
    if (!window) {
        std::cout << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::cout << "SDL_CreateRenderer failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    // Map tiles
    tilesetTexture = IMG_LoadTexture(renderer, "../assets/tileset.png");
    if (!tilesetTexture) {
        std::cout << "Failed to load tileset texture: " << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    // Load TMX map
    if (!map.load("../assets/map.tmx")) {
        std::cout << "Failed to load TMX map.\n";
        SDL_DestroyTexture(tilesetTexture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    std::cout << "Map loaded successfully.\n";
    std::cout << "Map size: " << map.getWidth() << " x " << map.getHeight() << std::endl;
    std::cout << "Tile size: " << map.getTileWidth() << " x " << map.getTileHeight() << std::endl;
    std::cout << "Item spawn count: " << map.getItemSpawnPoints().size() << std::endl;
    std::cout << "Collider count: " << map.getColliders().size() << std::endl;

    // Load a player
    if (!player.init(renderer, "../assets/player.png", 100.0f, 100.0f)) {
        std::cout << "Failed to initialize player.\n";
        SDL_DestroyTexture(tilesetTexture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    // Load a camera
    camera.x = 0.0;
    camera.y = 0.0;
    camera.w = static_cast<float>(width);
    camera.h = static_cast<float>(height);

    // Load an enemy
    // starts at (300,300), patrolls from x = 300 x = 500
    if (!enemy.init(renderer, "../assets/enemy.png", 300.0f, 300.0f, 300.0f, 500.0f)) {
        std::cout << "Failed to initialize enemy. \n";
        return false;
    }

    // Previous tick
    lastCounter = SDL_GetTicks();

    isRunning = true;
    return true;
}

void Game::run() {
    while (isRunning) {
        handleEvents();
        update();
        render();
        SDL_Delay(16);
    }
}

void Game::handleEvents() {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            isRunning = false;
        }

        if (event.type == SDL_EVENT_KEY_DOWN) {
            if (event.key.scancode == SDL_SCANCODE_SPACE) {
                enemy.takeDamage(1);
            }
        }
    }

    const bool* keyStates = SDL_GetKeyboardState(nullptr);
    player.handleInput(keyStates);
}

void Game::update() {
    // Camera
    Uint64 currentCounter = SDL_GetTicks();
    float deltaTime = (currentCounter - lastCounter) / 1000.0f;
    lastCounter = currentCounter;

    float mapPixelWidth = static_cast<float>(map.getWidth() * map.getTileWidth());
    float mapPixelHeight = static_cast<float>(map.getHeight() * map.getTileHeight());

    player.update(deltaTime, mapPixelWidth, mapPixelHeight);

    camera.x = player.getX() + player.getWidth() / 2.0f - camera.w / 2.0f;
    camera.y = player.getY() + player.getHeight() / 2.0f - camera.h / 2.0f;

    if (mapPixelWidth <= camera.w) {
        camera.x = 0.0f;
    } else {
        if (camera.x < 0.0f) camera.x = 0.0f;
        if (camera.x > mapPixelWidth - camera.w) camera.x = mapPixelWidth - camera.w;
    }

    if (mapPixelHeight <= camera.h) {
        camera.y = 0.0f;
    } else {
        if (camera.y < 0.0f) camera.y = 0.0f;
        if (camera.y > mapPixelHeight - camera.h) camera.y = mapPixelHeight - camera.h;
    }

    // Camera debugging
    // std::cout << "Player: " << player.getX() << ", " << player.getY() << std::endl;
    // std::cout << "Camera: " << camera.x << ", " << camera.y << std::endl;
    // std::cout << "Map pixels: " << map.getWidth() * map.getTileWidth()
    //           << ", " << map.getHeight() * map.getTileHeight() << std::endl;

    // Enemy
    SDL_FRect playerBounds{player.getX(), player.getY(), player.getWidth(), player.getHeight()};
    SDL_FRect enemyBounds = enemy.getBounds();

    if (SDL_HasRectIntersectionFloat(&playerBounds, &enemyBounds)) {
        std::cout << "Enemy encountered! \n";
    }

    enemy.update(deltaTime, player.getX(), player.getY());
    // Enemy debugging
    if (enemy.getState() == EnemyState::Idle) {
        std::cout << "Enemy state: Idle\n";
    }
    else if (enemy.getState() == EnemyState::Patrol) {
        std::cout << "Enemy state: Patrol\n";
    }
    else if (enemy.getState() == EnemyState::Chase) {
        std::cout << "Enemy state: Chase\n";
    }

}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 25, 25, 25, 255);
    SDL_RenderClear(renderer);
    SDL_FRect camRect{camera.x, camera.y, camera.w, camera.h};

    map.render(renderer, tilesetTexture, camRect);
    player.render(renderer, camRect);
    enemy.render(renderer, camRect);
    // Draw collision rectangles in red
    // SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    // for (const SDL_FRect& rect : map.getColliders()) {
    //     SDL_RenderRect(renderer, &rect);
    // }

    // Draw item points in green
    // SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    // for (const SpawnPoint& p : map.getItemSpawnPoints()) {
    //     SDL_FRect pointRect;
    //     pointRect.x = p.x - 4.0f;
    //     pointRect.y = p.y - 4.0f;
    //     pointRect.w = 8.0f;
    //     pointRect.h = 8.0f;
    //     SDL_RenderFillRect(renderer, &pointRect);
    // }

    SDL_RenderPresent(renderer);
}

void Game::clean() {
    if (tilesetTexture) {
        SDL_DestroyTexture(tilesetTexture);
        tilesetTexture = nullptr;
    }

    player.clean();
    enemy.clean();

    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    SDL_Quit();
}