#include "Game.h"
#include <iostream>

Game::Game()
    : window(nullptr),
      renderer(nullptr),
      tilesetTexture(nullptr),
      coinCount(0),
shootCooldown(0.2f),
shootTimer(0.0f),
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
    if (!map.load("../assets/map1_boss.tmx")) {
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
    std::cout << "Item spawn count: " << map.getItemSpawns().size() << std::endl;
    std::cout << "Collider count: " << map.getColliders().size() << std::endl;

    // Load a player
    float playerStartX = 100.0f;
    float playerStartY = 100.0f;

    if (map.hasPlayerSpawn()) {
        SpawnPoint spawn = map.getPlayerSpawn();
        playerStartX = spawn.x;
        playerStartY = spawn.y;
    }

    if (!player.init(renderer, "../assets/player.png", playerStartX, playerStartY)) {
        std::cout << "Failed to initialize player.\n";
        return false;
    }

    // Load a camera
    camera.x = 0.0;
    camera.y = 0.0;
    camera.w = static_cast<float>(width);
    camera.h = static_cast<float>(height);

    // Load enemies
    const std::vector<EnemySpawn>& enemySpawns = map.getEnemySpawns();

    for (const EnemySpawn& spawn : enemySpawns) {
        EnemyType type = stringToEnemyType(spawn.type);

        enemies.emplace_back();

        float patrolLeft = spawn.x - 100.0f;
        float patrolRight = spawn.x + 100.0f;

        if (!enemies.back().init(renderer,
                             enemyTexturePath(type),
                             spawn.x, spawn.y,
                             patrolLeft, patrolRight,
                             type)) {
            std::cout << "Failed to initialize one enemy.\n";
            enemies.pop_back();
        }
    }

    // Load items
    const std::vector<ItemSpawn>& itemSpawns = map.getItemSpawns();

    for (const ItemSpawn& spawn : itemSpawns) {
        ItemType type = stringToItemType(spawn.type);

        items.emplace_back();

        if (!items.back().init(renderer, itemTexturePath(type), spawn.x, spawn.y, type)) {
            std::cout << "Failed to initialize one item.\n";
            items.pop_back();
        }
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

        // Bullet
        if (event.type == SDL_EVENT_KEY_DOWN) {
            if (event.key.scancode == SDL_SCANCODE_SPACE) {
                Bullet bullet;

                float bulletX = player.getX() + player.getWidth() / 2.0f;
                float bulletY = player.getY() + player.getHeight() / 2.0f;

                bullet.init(bulletX, bulletY, player.getFacingDirection());
                bullets.push_back(bullet);

                shootTimer = shootCooldown;
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

    player.update(deltaTime, map);

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
    for (Enemy& enemy : enemies) {
        enemy.update(deltaTime, player.getX(), player.getY());
    }

    // Bullets
    for (Bullet& bullet : bullets) {
        bullet.update(deltaTime);

        SDL_FRect bulletBounds = bullet.getBounds();

        if (shootTimer > 0.0f) {
            shootTimer -= deltaTime;
        }

        if (bulletBounds.x < 0.0f || bulletBounds.x > mapPixelWidth) {
            bullet.deactivate();
        }
    }

    for (Bullet& bullet : bullets) {
        if (!bullet.isActive()) {
            continue;
        }

        SDL_FRect bulletBounds = bullet.getBounds();

        for (Enemy& enemy : enemies) {
            if (enemy.isDead()) {
                continue;
            }

            SDL_FRect enemyBounds = enemy.getBounds();

            bool overlaps =
                bulletBounds.x < enemyBounds.x + enemyBounds.w &&
                bulletBounds.x + bulletBounds.w > enemyBounds.x &&
                bulletBounds.y < enemyBounds.y + enemyBounds.h &&
                bulletBounds.y + bulletBounds.h > enemyBounds.y;

            if (overlaps) {
                enemy.takeDamage(1);
                bullet.deactivate();
                break;
            }
        }
    }

    // Items
    SDL_FRect playerBounds;
    playerBounds.x = player.getX();
    playerBounds.y = player.getY();
    playerBounds.w = player.getWidth();
    playerBounds.h = player.getHeight();

    for (Item& item : items) {
        if (!item.isActive()) {
            continue;
        }

        SDL_FRect itemBounds = item.getBounds();

        bool overlaps =
            playerBounds.x < itemBounds.x + itemBounds.w &&
            playerBounds.x + playerBounds.w > itemBounds.x &&
            playerBounds.y < itemBounds.y + itemBounds.h &&
            playerBounds.y + playerBounds.h > itemBounds.y;

        if (overlaps) {
            if (item.getType() == ItemType::Coin) {
                coinCount++;
                std::cout << "Picked up coin!\n";
            }
            else if (item.getType() == ItemType::Health) {
                std::cout << "Picked up health!\n";
            }

            item.pickUp();
        }
    }

}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 25, 25, 25, 255);
    SDL_RenderClear(renderer);
    SDL_FRect camRect{camera.x, camera.y, camera.w, camera.h};

    map.render(renderer, tilesetTexture, camRect);
    player.render(renderer, camRect);

    // Enemies
    for (Enemy& enemy : enemies) {
        enemy.render(renderer, camRect);
    }

    // Bullets
    for (Bullet& bullet : bullets) {
        bullet.render(renderer, camRect);
    }

    // Items
    for (Item& item : items) {
        item.render(renderer, camRect);
    }
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

bool Game::loadStage(const char* mapPath) {
    enemies.clear();
    items.clear();

    if (!map.load(mapPath)) {
        std::cout << "Failed to load map!: \n" << mapPath << std::endl;
        return false;
    }

    float playerStartX = 100.0f;
    float playerStartY = 100.0f;

    if (map.hasPlayerSpawn()) {
        SpawnPoint spawn = map.getPlayerSpawn();
        playerStartX = spawn.x;
        playerStartY = spawn.y;
    }

    // Player rest/reposition goes here

    // Enemies
    const std::vector<EnemySpawn>& enemySpawns = map.getEnemySpawns();
    for (const EnemySpawn& spawn : enemySpawns) {
        EnemyType type = stringToEnemyType(spawn.type);

        enemies.emplace_back();

        float patrolLeft = spawn.x - 100.0f;
        float patrolRight = spawn.x + 100.0f;

        if (!enemies.back().init(renderer,
                             enemyTexturePath(type),
                             spawn.x, spawn.y,
                             patrolLeft, patrolRight,
                             type)) {
            std::cout << "Failed to initialize one enemy.\n";
            enemies.pop_back();
                             }
    }

    // Items
    const std::vector<ItemSpawn>& itemSpawns = map.getItemSpawns();
    for (const ItemSpawn& spawn: itemSpawns) {
        ItemType type = stringToItemType(spawn.type);

        items.emplace_back();

        if (!items.back().init(renderer, itemTexturePath(type), spawn.x, spawn.y, type)) {
            items.pop_back();
        }
    }

    return true;
}
void Game::clean() {
    if (tilesetTexture) {
        SDL_DestroyTexture(tilesetTexture);
        tilesetTexture = nullptr;
    }

    player.clean();

    for (Enemy& enemy : enemies) {
        enemy.clean();
    }
    enemies.clear();

    for (Bullet& bullet : bullets) {
        bullet.deactivate();
    }

    for (Item& item : items) {
        item.clean();
    }
    items.clear();

    // Clear all drawings before renderer clearing
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