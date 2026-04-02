#include "Game.h"

Game::Game()
    : window(nullptr),
      renderer(nullptr),
      tilesetTexture(nullptr),
      doorTexture(nullptr),
      uiFont(nullptr),
      hpTextTexture(nullptr),
      hpTextRect{20.0f, 20.0f, 0.0f, 0.0f},
      ammoTextTexture(nullptr),
      ammoTextRect{20.0f, 50.0f, 0.0f, 0.0f},
      isRunning(false),
      lastCounter(0),
      shootCooldown(0.2f),
      shootTimer(0.0f),
      doorTimer(0.0f),
      doorCooldown(0.5f),
      coinCount(0),
      playerEntity(nullptr) {
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

    if (!TTF_Init()) {
        std::cout << "TTF_Init failed: " << SDL_GetError() << std::endl;
        return false;
    }

    uiFont = TTF_OpenFont("../assets/Fonts/PixelifySans_Font.ttf", 24.0f);
    if (!uiFont) {
        std::cout << "Failed to load font: " << SDL_GetError() << std::endl;
        return false;
    }

    tilesetTexture = IMG_LoadTexture(renderer, "../assets/Maps/tileset.png");
    if (!tilesetTexture) {
        std::cout << "Failed to load tileset texture: " << SDL_GetError() << std::endl;
        return false;
    }

    doorTexture = IMG_LoadTexture(renderer, "../assets/door.png");
    if (!doorTexture) {
        std::cout << "Failed to load door texture: " << SDL_GetError() << std::endl;
        return false;
    }

    camera.x = 0.0f;
    camera.y = 0.0f;
    camera.w = static_cast<float>(width);
    camera.h = static_cast<float>(height);

    if (!loadStage("../assets/Maps/map1_boss.tmx")) {
        return false;
    }

    std::cout << "Map loaded successfully.\n";
    std::cout << "Map size: " << map.getWidth() << " x " << map.getHeight() << std::endl;
    std::cout << "Tile size: " << map.getTileWidth() << " x " << map.getTileHeight() << std::endl;
    std::cout << "Item spawn count: " << map.getItemSpawns().size() << std::endl;
    std::cout << "Collider count: " << map.getColliders().size() << std::endl;
    std::cout << "Door count: " << map.getDoors().size() << std::endl;

    for (const DoorSpawn& door : map.getDoors()) {
        std::cout << "Door: x=" << door.x
                  << " y=" << door.y
                  << " w=" << door.w
                  << " h=" << door.h
                  << " target=" << door.targetMap
                  << std::endl;
    }

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
    if (!playerEntity) {
        return;
    }

    auto& input = playerEntity->getComponent<InputComponent>();
    auto& weapon = playerEntity->getComponent<WeaponComponent>();
    auto& transform = playerEntity->getComponent<TransformComponent>();

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            isRunning = false;
        }

        if (event.type == SDL_EVENT_KEY_DOWN) {
            if (event.key.scancode == SDL_SCANCODE_R) {
                weapon.startReload();
            }

            if (event.key.scancode == SDL_SCANCODE_SPACE &&
                shootTimer <= 0.0f &&
                weapon.canShoot()) {
                Bullet bullet;

                float bulletX = transform.x + transform.w / 2.0f;
                float bulletY = transform.y + transform.h / 2.0f;

                bullet.init(bulletX, bulletY, weapon.getFacingDirection());
                bullets.push_back(bullet);

                weapon.consumeAmmo();
                shootTimer = shootCooldown;
            }
        }
    }

    const bool* keyStates = SDL_GetKeyboardState(nullptr);
    input.handleInput(keyStates);
}

void Game::update() {
    if (!playerEntity) {
        return;
    }

    auto& transform = playerEntity->getComponent<TransformComponent>();
    auto& physics = playerEntity->getComponent<PhysicsComponent>();
    auto& health = playerEntity->getComponent<HealthComponent>();
    auto& weapon = playerEntity->getComponent<WeaponComponent>();

    Uint64 currentCounter = SDL_GetTicks();
    float deltaTime = (currentCounter - lastCounter) / 1000.0f;
    lastCounter = currentCounter;

    if (shootTimer > 0.0f) {
        shootTimer -= deltaTime;
    }

    if (doorTimer > 0.0f) {
        doorTimer -= deltaTime;
    }

    float mapPixelWidth = static_cast<float>(map.getWidth() * map.getTileWidth());
    float mapPixelHeight = static_cast<float>(map.getHeight() * map.getTileHeight());

    physics.update(deltaTime, map);
    weapon.update(deltaTime);

    // Update enemies
    for (Enemy& enemy : enemies) {
        enemy.update(deltaTime, transform.x, transform.y, map);
    }

    // Spawn enemy bullets
    for (Enemy& enemy : enemies) {
        if (enemy.isDead()) {
            continue;
        }

        if (!enemy.didShootThisFrame()) {
            continue;
        }

        SDL_FRect enemyBounds = enemy.getBounds();

        EnemyBullet bullet;
        int dir = (transform.x < enemyBounds.x) ? -1 : 1;

        float bulletX = enemyBounds.x + enemyBounds.w / 2.0f + dir * 20.0f;
        float bulletY = enemyBounds.y + enemyBounds.h / 2.0f;

        bullet.init(bulletX, bulletY, dir);
        enemyBullets.push_back(bullet);

        enemy.resetShotThisFrame();
    }

    // Update enemy bullets
    for (EnemyBullet& bullet : enemyBullets) {
        bullet.update(deltaTime);

        SDL_FRect bounds = bullet.getBounds();
        if (bounds.x < 0.0f || bounds.x > mapPixelWidth) {
            bullet.deactivate();
        }
    }

    // Camera follow
    camera.x = transform.x + transform.w / 2.0f - camera.w / 2.0f;
    camera.y = transform.y + transform.h / 2.0f - camera.h / 2.0f;

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

    SDL_FRect playerBounds{transform.x, transform.y, transform.w, transform.h};

    // Enemy melee/contact damage
    for (Enemy& enemy : enemies) {
        if (enemy.isDead()) {
            continue;
        }

        if (!enemy.didAttackHit()) {
            continue;
        }

        SDL_FRect enemyBounds = enemy.getBounds();

        bool overlaps =
            playerBounds.x < enemyBounds.x + enemyBounds.w &&
            playerBounds.x + playerBounds.w > enemyBounds.x &&
            playerBounds.y < enemyBounds.y + enemyBounds.h &&
            playerBounds.y + playerBounds.h > enemyBounds.y;

        if (overlaps) {
            health.takeDamage(1);
            std::cout << "Player took 1 damage. HP: " << health.getHP() << std::endl;
        }

        enemy.resetAttackHit();
    }

    // Update player bullets
    for (Bullet& bullet : bullets) {
        bullet.update(deltaTime);

        SDL_FRect bulletBounds = bullet.getBounds();
        if (bulletBounds.x < 0.0f || bulletBounds.x > mapPixelWidth) {
            bullet.deactivate();
        }
    }

    // Player bullets hit enemy
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

    // Item pickup
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
                std::cout << "Picked up coin. Coins: " << coinCount << std::endl;
            }
            else if (item.getType() == ItemType::Health) {
                health.heal(1);
                std::cout << "Picked up health. HP: " << health.getHP() << std::endl;
            }

            item.pickUp();
        }
    }

    // Door transition
    for (const DoorSpawn& door : map.getDoors()) {
        SDL_FRect doorRect{door.x, door.y, door.w, door.h};

        bool overlaps =
            playerBounds.x < doorRect.x + doorRect.w &&
            playerBounds.x + playerBounds.w > doorRect.x &&
            playerBounds.y < doorRect.y + doorRect.h &&
            playerBounds.y + playerBounds.h > doorRect.y;

        if (overlaps) {
            std::cout << "Player stepped on door! Target map: " << door.targetMap << std::endl;
        }

        if (overlaps && doorTimer <= 0.0f && !door.targetMap.empty()) {
            if (loadStage(door.targetMap.c_str())) {
                doorTimer = doorCooldown;
            }
            break;
        }
    }

    // Enemy bullets damage player
    for (EnemyBullet& bullet : enemyBullets) {
        if (!bullet.isActive()) {
            continue;
        }

        SDL_FRect bulletBounds = bullet.getBounds();

        bool overlaps =
            playerBounds.x < bulletBounds.x + bulletBounds.w &&
            playerBounds.x + playerBounds.w > bulletBounds.x &&
            playerBounds.y < bulletBounds.y + bulletBounds.h &&
            playerBounds.y + playerBounds.h > bulletBounds.y;

        if (overlaps) {
            health.takeDamage(1);
            bullet.deactivate();
            std::cout << "Player hit by enemy bullet. HP: " << health.getHP() << std::endl;
        }
    }

    bullets.erase(
        std::remove_if(bullets.begin(), bullets.end(),
                       [](const Bullet& bullet) { return !bullet.isActive(); }),
        bullets.end());

    enemyBullets.erase(
        std::remove_if(enemyBullets.begin(), enemyBullets.end(),
                       [](const EnemyBullet& bullet) { return !bullet.isActive(); }),
        enemyBullets.end());
}

bool Game::updateHPText() {
    if (!playerEntity) {
        return false;
    }

    auto& health = playerEntity->getComponent<HealthComponent>();

    if (hpTextTexture) {
        SDL_DestroyTexture(hpTextTexture);
        hpTextTexture = nullptr;
    }

    std::string hpString = "HP: " + std::to_string(health.getHP());

    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface* textSurface = TTF_RenderText_Blended(uiFont, hpString.c_str(), 0, color);
    if (!textSurface) {
        std::cout << "Failed to render HP text surface: " << SDL_GetError() << std::endl;
        return false;
    }

    hpTextTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (!hpTextTexture) {
        std::cout << "Failed to create HP text texture: " << SDL_GetError() << std::endl;
        SDL_DestroySurface(textSurface);
        return false;
    }

    hpTextRect.w = static_cast<float>(textSurface->w);
    hpTextRect.h = static_cast<float>(textSurface->h);

    SDL_DestroySurface(textSurface);
    return true;
}

bool Game::updateAmmoText() {
    if (!playerEntity) {
        return false;
    }

    auto& weapon = playerEntity->getComponent<WeaponComponent>();

    if (ammoTextTexture) {
        SDL_DestroyTexture(ammoTextTexture);
        ammoTextTexture = nullptr;
    }

    std::string ammoString = "Ammo: " + std::to_string(weapon.getAmmo()) +
                             "/" + std::to_string(weapon.getMaxAmmo());

    if (weapon.isReloading()) {
        ammoString += " (Reloading)";
    }

    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface* textSurface = TTF_RenderText_Blended(uiFont, ammoString.c_str(), 0, color);
    if (!textSurface) {
        return false;
    }

    ammoTextTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (!ammoTextTexture) {
        SDL_DestroySurface(textSurface);
        return false;
    }

    ammoTextRect.w = static_cast<float>(textSurface->w);
    ammoTextRect.h = static_cast<float>(textSurface->h);

    SDL_DestroySurface(textSurface);
    return true;
}

void Game::render() {
    if (!playerEntity) {
        return;
    }

    SDL_SetRenderDrawColor(renderer, 25, 25, 25, 255);
    SDL_RenderClear(renderer);

    SDL_FRect camRect{camera.x, camera.y, camera.w, camera.h};

    updateHPText();
    updateAmmoText();

    map.render(renderer, tilesetTexture, camRect);

    for (const DoorSpawn& door : map.getDoors()) {
        SDL_FRect dstRect;
        dstRect.x = door.x - camera.x;
        dstRect.y = door.y - camera.y;
        dstRect.w = door.w;
        dstRect.h = door.h;

        SDL_RenderTexture(renderer, doorTexture, nullptr, &dstRect);
    }

    for (Item& item : items) {
        item.render(renderer, camRect);
    }

    for (Enemy& enemy : enemies) {
        enemy.render(renderer, camRect);
    }

    for (EnemyBullet& bullet : enemyBullets) {
        bullet.render(renderer, camRect);
    }

    for (Bullet& bullet : bullets) {
        bullet.render(renderer, camRect);
    }

    playerEntity->getComponent<SpriteComponent>().draw(camRect);

    if (hpTextTexture) {
        SDL_RenderTexture(renderer, hpTextTexture, nullptr, &hpTextRect);
    }

    if (ammoTextTexture) {
        SDL_RenderTexture(renderer, ammoTextTexture, nullptr, &ammoTextRect);
    }

    SDL_RenderPresent(renderer);
}

bool Game::loadStage(const char* mapPath) {
    for (Enemy& enemy : enemies) {
        enemy.clean();
    }
    enemies.clear();
    enemyBullets.clear();

    for (Item& item : items) {
        item.clean();
    }
    items.clear();

    bullets.clear();

    if (!map.load(mapPath)) {
        std::cout << "Failed to load map: " << mapPath << std::endl;
        return false;
    }

    // Recreate player entity only once if needed
    if (!playerEntity) {
        playerEntity = &manager.addEntity();

        float startX = 100.0f;
        float startY = 100.0f;

        if (map.hasPlayerSpawn()) {
            SpawnPoint spawn = map.getPlayerSpawn();
            startX = spawn.x;
            startY = spawn.y;
        }

        PlayerFactory::createPlayer(*playerEntity, renderer, startX, startY);
    }
    else {
        auto& transform = playerEntity->getComponent<TransformComponent>();

        if (map.hasPlayerSpawn()) {
            SpawnPoint spawn = map.getPlayerSpawn();
            transform.setPosition(spawn.x, spawn.y);
        } else {
            transform.setPosition(100.0f, 100.0f);
        }
    }

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

    const std::vector<ItemSpawn>& itemSpawns = map.getItemSpawns();
    for (const ItemSpawn& spawn : itemSpawns) {
        ItemType type = stringToItemType(spawn.type);

        items.emplace_back();

        if (!items.back().init(renderer, itemTexturePath(type), spawn.x, spawn.y, type)) {
            std::cout << "Failed to initialize one item.\n";
            items.pop_back();
        }
    }

    std::cout << "Loaded stage: " << mapPath << std::endl;
    std::cout << "Enemies spawned: " << enemies.size() << std::endl;
    std::cout << "Items spawned: " << items.size() << std::endl;

    return true;
}

void Game::clean() {
    if (tilesetTexture) {
        SDL_DestroyTexture(tilesetTexture);
        tilesetTexture = nullptr;
    }

    if (doorTexture) {
        SDL_DestroyTexture(doorTexture);
        doorTexture = nullptr;
    }

    if (ammoTextTexture) {
        SDL_DestroyTexture(ammoTextTexture);
        ammoTextTexture = nullptr;
    }

    if (hpTextTexture) {
        SDL_DestroyTexture(hpTextTexture);
        hpTextTexture = nullptr;
    }

    for (Enemy& enemy : enemies) {
        enemy.clean();
    }
    enemies.clear();

    enemyBullets.clear();
    bullets.clear();

    for (Item& item : items) {
        item.clean();
    }
    items.clear();

    playerEntity = nullptr;

    if (uiFont) {
        TTF_CloseFont(uiFont);
        uiFont = nullptr;
    }

    TTF_Quit();

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