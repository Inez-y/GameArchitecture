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

    auto& playerTransform = playerEntity->getComponent<TransformComponent>();
    auto& playerPhysics = playerEntity->getComponent<PhysicsComponent>();
    auto& playerHealth = playerEntity->getComponent<HealthComponent>();
    auto& playerWeapon = playerEntity->getComponent<WeaponComponent>();

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

    // Update player ECS
    playerPhysics.update(deltaTime, map);
    playerWeapon.update(deltaTime);

    // --------------------
    // UPDATE ECS ENEMIES
    // --------------------
    for (auto& e : manager.getEntities()) {
        if (!e->hasComponent<EnemyTagComponent>()) {
            continue;
        }

        auto& transform = e->getComponent<TransformComponent>();
        auto& physics = e->getComponent<PhysicsComponent>();
        auto& health = e->getComponent<HealthComponent>();
        auto& ai = e->getComponent<EnemyAIComponent>();

        ai.beginFrame();

        if (health.isDead()) {
            ai.changeState(EnemyState::Dead);
        }

        switch (ai.type) {
            case EnemyType::Patrol:
                switch (ai.state) {
                    case EnemyState::Idle:
                        physics.setMoveX(0.0f);
                        if (ai.distanceTo(transform.x, transform.y, playerTransform.x, playerTransform.y) <= ai.attackRange) {
                            ai.changeState(EnemyState::Attack);
                        } else if (ai.distanceTo(transform.x, transform.y, playerTransform.x, playerTransform.y) <= ai.chaseRange) {
                            ai.changeState(EnemyState::Chase);
                        } else {
                            ai.idleTimer += deltaTime;
                            if (ai.idleTimer >= ai.idleDuration) {
                                ai.changeState(EnemyState::Patrol);
                            }
                        }
                        break;

                    case EnemyState::Patrol:
                        if (ai.distanceTo(transform.x, transform.y, playerTransform.x, playerTransform.y) <= ai.attackRange) {
                            ai.changeState(EnemyState::Attack);
                        } else if (ai.distanceTo(transform.x, transform.y, playerTransform.x, playerTransform.y) <= ai.chaseRange) {
                            ai.changeState(EnemyState::Chase);
                        } else {
                            physics.setMoveX(static_cast<float>(ai.direction));

                            if (transform.x <= ai.patrolLeft) {
                                transform.x = ai.patrolLeft;
                                ai.direction = 1;
                                ai.changeState(EnemyState::Idle);
                            } else if (transform.x >= ai.patrolRight) {
                                transform.x = ai.patrolRight;
                                ai.direction = -1;
                                ai.changeState(EnemyState::Idle);
                            }
                        }
                        break;

                    case EnemyState::Chase:
                        if (ai.distanceTo(transform.x, transform.y, playerTransform.x, playerTransform.y) <= ai.attackRange) {
                            ai.changeState(EnemyState::Attack);
                        } else if (ai.distanceTo(transform.x, transform.y, playerTransform.x, playerTransform.y) >= ai.stopChaseRange) {
                            ai.changeState(EnemyState::Patrol);
                        } else {
                            ai.direction = (playerTransform.x < transform.x) ? -1 : 1;
                            physics.setMoveX(static_cast<float>(ai.direction));
                        }
                        break;

                    case EnemyState::Attack:
                        physics.setMoveX(0.0f);
                        ai.attackTimer += deltaTime;

                        if (!ai.damageAppliedThisAttack && ai.attackTimer >= ai.attackDuration * 0.5f) {
                            ai.damageAppliedThisAttack = true;
                            ai.attackHitThisFrame = true;
                        }

                        if (ai.attackTimer >= ai.attackDuration) {
                            float dist = ai.distanceTo(transform.x, transform.y, playerTransform.x, playerTransform.y);
                            if (dist <= ai.attackRange) ai.changeState(EnemyState::Attack);
                            else if (dist <= ai.chaseRange) ai.changeState(EnemyState::Chase);
                            else ai.changeState(EnemyState::Patrol);
                        }
                        break;

                    case EnemyState::Hurt:
                        physics.setMoveX(0.0f);
                        ai.hurtTimer += deltaTime;
                        if (ai.hurtTimer >= ai.hurtDuration) {
                            if (health.isDead()) ai.changeState(EnemyState::Dead);
                            else ai.changeState(EnemyState::Patrol);
                        }
                        break;

                    case EnemyState::Dead:
                        physics.setMoveX(0.0f);
                        break;
                }
                break;

            case EnemyType::Shooter:
                switch (ai.state) {
                    case EnemyState::Idle:
                        physics.setMoveX(0.0f);
                        if (ai.distanceTo(transform.x, transform.y, playerTransform.x, playerTransform.y) <= ai.chaseRange) {
                            ai.changeState(EnemyState::Attack);
                        }
                        break;

                    case EnemyState::Attack:
                        physics.setMoveX(0.0f);
                        ai.direction = (playerTransform.x < transform.x) ? -1 : 1;
                        ai.attackTimer += deltaTime;

                        if (!ai.damageAppliedThisAttack && ai.attackTimer >= 0.5f) {
                            ai.damageAppliedThisAttack = true;
                            ai.shotThisFrame = true;
                        }

                        if (ai.attackTimer >= 1.0f) {
                            ai.changeState(EnemyState::Idle);
                        }
                        break;

                    case EnemyState::Hurt:
                        physics.setMoveX(0.0f);
                        ai.hurtTimer += deltaTime;
                        if (ai.hurtTimer >= ai.hurtDuration) {
                            if (health.isDead()) ai.changeState(EnemyState::Dead);
                            else ai.changeState(EnemyState::Idle);
                        }
                        break;

                    case EnemyState::Dead:
                        physics.setMoveX(0.0f);
                        break;

                    default:
                        ai.changeState(EnemyState::Idle);
                        break;
                }
                break;

            case EnemyType::Flying:
                switch (ai.state) {
                    case EnemyState::Chase: {
                        float dx = playerTransform.x - transform.x;
                        float dy = playerTransform.y - transform.y;

                        ai.direction = (dx < 0.0f) ? -1 : 1;
                        transform.x += ai.direction * physics.getSpeed() * deltaTime * 0.8f;

                        if (dy < -10.0f) transform.y -= physics.getSpeed() * deltaTime * 0.5f;
                        else if (dy > 10.0f) transform.y += physics.getSpeed() * deltaTime * 0.5f;

                        if (ai.distanceTo(transform.x, transform.y, playerTransform.x, playerTransform.y) <= ai.attackRange) {
                            ai.changeState(EnemyState::Attack);
                        }
                        break;
                    }

                    case EnemyState::Attack: {
                        float dx = playerTransform.x - transform.x;
                        float dy = playerTransform.y - transform.y;

                        ai.direction = (dx < 0.0f) ? -1 : 1;
                        transform.x += ai.direction * physics.getSpeed() * deltaTime * 1.2f;

                        if (dy < -10.0f) transform.y -= physics.getSpeed() * deltaTime * 0.8f;
                        else if (dy > 10.0f) transform.y += physics.getSpeed() * deltaTime * 0.8f;

                        ai.attackTimer += deltaTime;

                        if (!ai.damageAppliedThisAttack && ai.attackTimer >= 0.3f) {
                            ai.damageAppliedThisAttack = true;
                            ai.attackHitThisFrame = true;
                        }

                        if (ai.attackTimer >= 0.7f) {
                            ai.changeState(EnemyState::Chase);
                        }
                        break;
                    }

                    case EnemyState::Hurt:
                        ai.hurtTimer += deltaTime;
                        if (ai.hurtTimer >= ai.hurtDuration) {
                            if (health.isDead()) ai.changeState(EnemyState::Dead);
                            else ai.changeState(EnemyState::Chase);
                        }
                        break;

                    case EnemyState::Dead:
                        break;

                    default:
                        ai.changeState(EnemyState::Chase);
                        break;
                }
                break;

            case EnemyType::Boss:
                switch (ai.state) {
                    case EnemyState::Idle:
                        physics.setMoveX(0.0f);
                        ai.bossAttackTimer += deltaTime;

                        if (ai.bossAttackTimer >= ai.bossAttackCooldown) {
                            ai.bossAttackTimer = 0.0f;
                            ai.bossAttackPhase = 0;

                            float dist = ai.distanceTo(transform.x, transform.y, playerTransform.x, playerTransform.y);
                            if (dist > 350.0f) ai.currentBossAttack = BossAttackType::BurstShot;
                            else ai.currentBossAttack = BossAttackType::DashAttack;

                            ai.changeState(EnemyState::Attack);
                        }
                        break;

                    case EnemyState::Attack:
                        if (ai.currentBossAttack == BossAttackType::BurstShot) {
                            physics.setMoveX(0.0f);
                            ai.bossAttackTimer += deltaTime;
                            ai.direction = (playerTransform.x < transform.x) ? -1 : 1;

                            if (ai.bossAttackPhase == 0) {
                                if (ai.bossAttackTimer >= 0.5f) {
                                    ai.bossAttackTimer = 0.0f;
                                    ai.bossAttackPhase = 1;
                                }
                            } else if (ai.bossAttackPhase == 1) {
                                ai.shotThisFrame = true;
                                ai.bossAttackTimer = 0.0f;
                                ai.bossAttackPhase = 2;
                            } else if (ai.bossAttackPhase == 2) {
                                if (ai.bossAttackTimer >= 0.2f) {
                                    ai.shotThisFrame = true;
                                    ai.bossAttackTimer = 0.0f;
                                    ai.bossAttackPhase = 3;
                                }
                            } else if (ai.bossAttackPhase == 3) {
                                if (ai.bossAttackTimer >= 0.2f) {
                                    ai.shotThisFrame = true;
                                    ai.bossAttackTimer = 0.0f;
                                    ai.bossAttackPhase = 4;
                                }
                            } else if (ai.bossAttackPhase == 4) {
                                if (ai.bossAttackTimer >= 0.4f) {
                                    ai.bossAttackTimer = 0.0f;
                                    ai.currentBossAttack = BossAttackType::None;
                                    ai.changeState(EnemyState::Idle);
                                }
                            }
                        }
                        else if (ai.currentBossAttack == BossAttackType::DashAttack) {
                            ai.bossAttackTimer += deltaTime;
                            ai.direction = (playerTransform.x < transform.x) ? -1 : 1;

                            if (ai.bossAttackPhase == 0) {
                                physics.setMoveX(0.0f);
                                if (ai.bossAttackTimer >= 0.4f) {
                                    ai.bossAttackTimer = 0.0f;
                                    ai.bossAttackPhase = 1;
                                }
                            } else if (ai.bossAttackPhase == 1) {
                                physics.setMoveX(static_cast<float>(ai.direction) * 3.0f);
                                ai.attackHitThisFrame = true;

                                if (ai.bossAttackTimer >= 0.5f) {
                                    ai.bossAttackTimer = 0.0f;
                                    ai.bossAttackPhase = 2;
                                }
                            } else if (ai.bossAttackPhase == 2) {
                                physics.setMoveX(0.0f);
                                if (ai.bossAttackTimer >= 0.5f) {
                                    ai.bossAttackTimer = 0.0f;
                                    ai.currentBossAttack = BossAttackType::None;
                                    ai.changeState(EnemyState::Idle);
                                }
                            }
                        }
                        break;

                    case EnemyState::Hurt:
                        physics.setMoveX(0.0f);
                        ai.hurtTimer += deltaTime;
                        if (ai.hurtTimer >= ai.hurtDuration) {
                            if (health.isDead()) ai.changeState(EnemyState::Dead);
                            else ai.changeState(EnemyState::Idle);
                        }
                        break;

                    case EnemyState::Dead:
                        physics.setMoveX(0.0f);
                        break;

                    default:
                        ai.changeState(EnemyState::Idle);
                        break;
                }
                break;
        }

        physics.update(deltaTime, map);

        if (health.isDead()) {
            ai.changeState(EnemyState::Dead);
        }
    }

    // Spawn enemy bullets
    for (auto& e : manager.getEntities()) {
        if (!e->hasComponent<EnemyTagComponent>()) {
            continue;
        }

        auto& transform = e->getComponent<TransformComponent>();
        auto& ai = e->getComponent<EnemyAIComponent>();
        auto& health = e->getComponent<HealthComponent>();

        if (health.isDead() || !ai.shotThisFrame) {
            continue;
        }

        EnemyBullet bullet;
        int dir = (playerTransform.x < transform.x) ? -1 : 1;

        float bulletX = transform.x + transform.w / 2.0f + dir * 20.0f;
        float bulletY = transform.y + transform.h / 2.0f;

        bullet.init(bulletX, bulletY, dir);
        enemyBullets.push_back(bullet);

        ai.resetShotThisFrame();
    }

    for (EnemyBullet& bullet : enemyBullets) {
        bullet.update(deltaTime);

        SDL_FRect bounds = bullet.getBounds();
        if (bounds.x < 0.0f || bounds.x > mapPixelWidth) {
            bullet.deactivate();
        }
    }

    // Camera
    camera.x = playerTransform.x + playerTransform.w / 2.0f - camera.w / 2.0f;
    camera.y = playerTransform.y + playerTransform.h / 2.0f - camera.h / 2.0f;

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

    SDL_FRect playerBounds{playerTransform.x, playerTransform.y, playerTransform.w, playerTransform.h};

    // Enemy melee/contact damage
    for (auto& e : manager.getEntities()) {
        if (!e->hasComponent<EnemyTagComponent>()) {
            continue;
        }

        auto& transform = e->getComponent<TransformComponent>();
        auto& ai = e->getComponent<EnemyAIComponent>();
        auto& health = e->getComponent<HealthComponent>();

        if (health.isDead() || !ai.attackHitThisFrame) {
            continue;
        }

        SDL_FRect enemyBounds = transform.getRect();

        bool overlaps =
            playerBounds.x < enemyBounds.x + enemyBounds.w &&
            playerBounds.x + playerBounds.w > enemyBounds.x &&
            playerBounds.y < enemyBounds.y + enemyBounds.h &&
            playerBounds.y + playerBounds.h > enemyBounds.y;

        if (overlaps) {
            playerHealth.takeDamage(1);
            std::cout << "Player took 1 damage. HP: " << playerHealth.getHP() << std::endl;
        }

        ai.resetAttackHit();
    }

    // Player bullets
    for (Bullet& bullet : bullets) {
        bullet.update(deltaTime);

        SDL_FRect bulletBounds = bullet.getBounds();
        if (bulletBounds.x < 0.0f || bulletBounds.x > mapPixelWidth) {
            bullet.deactivate();
        }
    }

    for (Bullet& bullet : bullets) {
        if (!bullet.isActive()) {
            continue;
        }

        SDL_FRect bulletBounds = bullet.getBounds();

        for (auto& e : manager.getEntities()) {
            if (!e->hasComponent<EnemyTagComponent>()) {
                continue;
            }

            auto& transform = e->getComponent<TransformComponent>();
            auto& ai = e->getComponent<EnemyAIComponent>();
            auto& health = e->getComponent<HealthComponent>();

            if (health.isDead()) {
                continue;
            }

            SDL_FRect enemyBounds = transform.getRect();

            bool overlaps =
                bulletBounds.x < enemyBounds.x + enemyBounds.w &&
                bulletBounds.x + bulletBounds.w > enemyBounds.x &&
                bulletBounds.y < enemyBounds.y + enemyBounds.h &&
                bulletBounds.y + bulletBounds.h > enemyBounds.y;

            if (overlaps) {
                health.takeDamage(1);

                if (health.isDead()) {
                    ai.changeState(EnemyState::Dead);
                } else {
                    ai.changeState(EnemyState::Hurt);
                }

                bullet.deactivate();
                break;
            }
        }
    }

    // Item pickup ONLY
    for (auto& e : manager.getEntities()) {
        if (!e->hasComponent<ItemTagComponent>()) {
            continue;
        }

        auto& transform = e->getComponent<TransformComponent>();
        auto& item = e->getComponent<ItemComponent>();

        if (!item.isActive()) {
            continue;
        }

        SDL_FRect itemBounds = transform.getRect();

        bool overlaps =
            playerBounds.x < itemBounds.x + itemBounds.w &&
            playerBounds.x + playerBounds.w > itemBounds.x &&
            playerBounds.y < itemBounds.y + itemBounds.h &&
            playerBounds.y + playerBounds.h > itemBounds.y;

        if (overlaps) {
            applyItemEffect(item.getType());
            item.pickUp();
            e->destroy();
        }
    }

    manager.refresh();

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

    // Enemy bullets hit player
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
            playerHealth.takeDamage(1);
            bullet.deactivate();
            std::cout << "Player hit by enemy bullet. HP: " << playerHealth.getHP() << std::endl;
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

    // Items
    for (auto& e : manager.getEntities()) {
        if (!e->hasComponent<ItemTagComponent>()) {
            continue;
        }

        auto& item = e->getComponent<ItemComponent>();
        if (!item.isActive()) {
            continue;
        }

        e->getComponent<SpriteComponent>().draw(camRect);
    }

    // Enemies
    for (auto& e : manager.getEntities()) {
        if (!e->hasComponent<EnemyTagComponent>()) {
            continue;
        }

        auto& health = e->getComponent<HealthComponent>();
        if (health.isDead()) {
            continue;
        }

        e->getComponent<SpriteComponent>().draw(camRect);
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
    for (auto& e : manager.getEntities()) {
        if (!e->hasComponent<PlayerTagComponent>()) {
            e->destroy();
        }
    }

    manager.refresh();

    enemyBullets.clear();
    bullets.clear();

    if (!map.load(mapPath)) {
        std::cout << "Failed to load map: " << mapPath << std::endl;
        return false;
    }

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
    } else {
        auto& transform = playerEntity->getComponent<TransformComponent>();

        if (map.hasPlayerSpawn()) {
            SpawnPoint spawn = map.getPlayerSpawn();
            transform.setPosition(spawn.x, spawn.y);
        } else {
            transform.setPosition(100.0f, 100.0f);
        }
    }

    // Spawn enemies once
    const std::vector<EnemySpawn>& enemySpawns = map.getEnemySpawns();
    for (const EnemySpawn& spawn : enemySpawns) {
        Entity& enemy = manager.addEntity();

        float patrolLeft = spawn.x - 100.0f;
        float patrolRight = spawn.x + 100.0f;

        EnemyFactory::createEnemy(enemy,
                                  renderer,
                                  spawn.type,
                                  spawn.x,
                                  spawn.y,
                                  patrolLeft,
                                  patrolRight);
    }

    // Spawn items once
    const std::vector<ItemSpawn>& itemSpawns = map.getItemSpawns();
    for (const ItemSpawn& spawn : itemSpawns) {
        Entity& item = manager.addEntity();
        ItemFactory::createItem(item, renderer, spawn.type, spawn.x, spawn.y);
    }

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

    enemyBullets.clear();
    bullets.clear();

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

void Game::applyItemEffect(ItemType type) {
    if (!playerEntity) {
        return;
    }

    auto& playerHealth = playerEntity->getComponent<HealthComponent>();

    switch (type) {
        case ItemType::Coin:
            coinCount++;
            std::cout << "Picked up coin. Coins: " << coinCount << std::endl;
            break;

        case ItemType::Health:
            playerHealth.heal(1);
            std::cout << "Picked up health. HP: " << playerHealth.getHP() << std::endl;
            break;

        case ItemType::Key:
            std::cout << "Picked up key.\n";
            break;
    }
}