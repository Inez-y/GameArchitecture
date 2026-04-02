#include "Enemy.h"
#include <SDL3_image/SDL_image.h>
#include <iostream>
#include <cmath>

Enemy::Enemy()
    : texture(nullptr),
      x(0.0f),
      y(0.0f),
grounded(false),
velocityY(0.0f),
gravity(900.0f),
      speed(100.0f),
      patrolLeft(0.0f),
      patrolRight(0.0f),
      direction(1),
      type(EnemyType::Patrol),
      state(EnemyState::Patrol),
      idleTimer(0.0f),
      idleDuration(1.0f),
      chaseRange(220.0f),
      stopChaseRange(260.0f),
      attackRange(40.0f),
      attackTimer(0.0f),
      attackDuration(0.6f),
      damageAppliedThisAttack(false),
      attackHitThisFrame(false),
      hurtTimer(0.0f),
      hurtDuration(0.35f),
      health(3),
      currentBossAttack(BossAttackType::None),
      bossAttackTimer(0.0f),
      bossAttackCooldown(1.5f),
      bossAttackPhase(0),
shotThisFrame(false){
    dstRect = {0.0f, 0.0f, 32.0f, 32.0f};
}

bool Enemy::init(SDL_Renderer* renderer, const char* texturePath,
                 float startX, float startY,
                 float leftBound, float rightBound,
                 EnemyType enemyType) {
    texture = IMG_LoadTexture(renderer, texturePath);
    if (!texture) {
        std::cout << "Failed to load texture: " << SDL_GetError() << std::endl;
        return false;
    }

    x = startX;
    y = startY;
    patrolLeft = leftBound;
    patrolRight = rightBound;
    type = enemyType;

    // Default stats
    speed = 100.0f;
    health = 3;

    // Per-type setup
    if (type == EnemyType::Boss) {
        speed = 140.0f;
        health = 20;
        attackRange = 60.0f;
        chaseRange = 260.0f;
        state = EnemyState::Idle;
    } else if (type == EnemyType::Shooter) {
        state = EnemyState::Idle;
        attackRange = 250.0f;
        chaseRange = 300.0f;
    } else if (type == EnemyType::Flying) {
        state = EnemyState::Chase;
        attackRange = 60.0f;
        chaseRange = 260.0f;
    } else {
        state = EnemyState::Patrol;
        attackRange = 40.0f;
        chaseRange = 220.0f;
    }

    float texW = 0.0f;
    float texH = 0.0f;
    SDL_GetTextureSize(texture, &texW, &texH);

    dstRect = {x, y, texW, texH};

    return true;
}

void Enemy::update(float deltaTime, float playerX, float playerY, const Map& map) {
    attackHitThisFrame = false;
    shotThisFrame = false;

    switch (type) {
        case EnemyType::Patrol:
            switch (state) {
                case EnemyState::Idle:
                    updateIdle(deltaTime, playerX, playerY);
                    break;
                case EnemyState::Patrol:
                    updatePatrol(deltaTime, playerX, playerY);
                    break;
                case EnemyState::Chase:
                    updateChase(deltaTime, playerX, playerY);
                    break;
                case EnemyState::Attack:
                    updatePatrolAttack(deltaTime, playerX, playerY);
                    break;
                case EnemyState::Hurt:
                    updateHurt(deltaTime, playerX, playerY);
                    break;
                case EnemyState::Dead:
                    updateDead(deltaTime, playerX, playerY);
                    break;
            }
            break;

        case EnemyType::Shooter:
            switch (state) {
                case EnemyState::Idle:
                    updateShooterIdle(deltaTime, playerX, playerY);
                    break;
                case EnemyState::Attack:
                    updateShooterAttack(deltaTime, playerX, playerY);
                    break;
                case EnemyState::Hurt:
                    updateHurt(deltaTime, playerX, playerY);
                    break;
                case EnemyState::Dead:
                    updateDead(deltaTime, playerX, playerY);
                    break;
                default:
                    changeState(EnemyState::Idle);
                    break;
            }
            break;

        case EnemyType::Flying:
            switch (state) {
                case EnemyState::Chase:
                    updateFlyingType(deltaTime, playerX, playerY);
                    break;
                case EnemyState::Attack:
                    updateFlyingAttack(deltaTime, playerX, playerY);
                    break;
                case EnemyState::Hurt:
                    updateHurt(deltaTime, playerX, playerY);
                    break;
                case EnemyState::Dead:
                    updateDead(deltaTime, playerX, playerY);
                    break;
                default:
                    changeState(EnemyState::Chase);
                    break;
            }
            break;

        case EnemyType::Boss:
            updateBoss(deltaTime, playerX, playerY);
            break;
    }

    applyGravity(deltaTime, map);

    dstRect.x = x;
    dstRect.y = y;
}

void Enemy::updateIdle(float deltaTime, float playerX, float playerY) {
    if (distanceToPlayer(playerX, playerY) <= attackRange) {
        changeState(EnemyState::Attack);
        return;
    }

    if (distanceToPlayer(playerX, playerY) <= chaseRange) {
        changeState(EnemyState::Chase);
        return;
    }

    idleTimer += deltaTime;
    if (idleTimer >= idleDuration) {
        changeState(EnemyState::Patrol);
    }
}

void Enemy::updatePatrol(float deltaTime, float playerX, float playerY) {
    float dist = distanceToPlayer(playerX, playerY);

    if (dist <= attackRange) {
        changeState(EnemyState::Attack);
        return;
    }

    if (dist <= chaseRange) {
        changeState(EnemyState::Chase);
        return;
    }

    x += direction * speed * deltaTime;

    if (x <= patrolLeft) {
        x = patrolLeft;
        direction = 1;
        changeState(EnemyState::Idle);
    } else if (x >= patrolRight) {
        x = patrolRight;
        direction = -1;
        changeState(EnemyState::Idle);
    }
}

void Enemy::updateChase(float deltaTime, float playerX, float playerY) {
    float dist = distanceToPlayer(playerX, playerY);

    if (dist <= attackRange) {
        changeState(EnemyState::Attack);
        return;
    }

    if (dist >= stopChaseRange) {
        changeState(EnemyState::Patrol);
        return;
    }

    if (playerX < x) {
        direction = -1;
    } else if (playerX > x) {
        direction = 1;
    }

    x += direction * speed * 1.2f * deltaTime;
}

void Enemy::updateAttack(float deltaTime, float playerX, float playerY) {
    float dist = distanceToPlayer(playerX, playerY);

    attackTimer += deltaTime;

    // Melee
    if (!damageAppliedThisAttack && attackTimer >= attackDuration * 0.5f) {
        damageAppliedThisAttack = true;
        attackHitThisFrame = true;
    }

    // Shooter
    if (!damageAppliedThisAttack && attackTimer >= attackDuration * 0.5f) {
        damageAppliedThisAttack = true;
        // attackHitThisFrame = true;
        shotThisFrame = true;
    }

    if (attackTimer >= attackDuration) {
        if (dist <= attackRange) {
            changeState(EnemyState::Attack);
        } else if (dist <= chaseRange) {
            changeState(EnemyState::Chase);
        } else {
            changeState(EnemyState::Patrol);
        }
    }
}

void Enemy::updateHurt(float deltaTime, float playerX, float playerY) {
    hurtTimer += deltaTime;

    if (hurtTimer >= hurtDuration) {
        float dist = distanceToPlayer(playerX, playerY);

        if (health <= 0) {
            changeState(EnemyState::Dead);
        } else if (dist <= attackRange) {
            changeState(EnemyState::Attack);
        } else if (dist <= chaseRange) {
            changeState(EnemyState::Chase);
        } else {
            changeState(EnemyState::Patrol);
        }
    }
}

void Enemy::updateDead(float deltaTime, float playerX, float playerY) {
    // Dead enemies do nothing
}

void Enemy::takeDamage(int damage) {
    if (state == EnemyState::Dead) {
        return;
    }

    health -= damage;

    if (health <= 0) {
        health = 0;
        changeState(EnemyState::Dead);
        std::cout << "Enemy died\n";
    } else {
        changeState(EnemyState::Hurt);
        std::cout << "Enemy hurt. Remaining HP: " << health << std::endl;
    }
}

void Enemy::changeState(EnemyState newState) {
    if (state == newState) {
        return;
    }

    state = newState;

    if (state == EnemyState::Idle) {
        idleTimer = 0.0f;
    } else if (state == EnemyState::Attack) {
        attackTimer = 0.0f;
        damageAppliedThisAttack = false;
    } else if (state == EnemyState::Hurt) {
        hurtTimer = 0.0f;
    }
}


// TYPE-SPECIFIC LOGIC
void Enemy::updateShooterType(float deltaTime, float playerX, float playerY) {
    float dist = distanceToPlayer(playerX, playerY);

    switch (state) {
        case EnemyState::Idle:
            if (dist <= chaseRange) {
                changeState(EnemyState::Attack);
            }
            break;

        case EnemyState::Attack:
            updateAttack(deltaTime, playerX, playerY);
            break;

        case EnemyState::Hurt:
            updateHurt(deltaTime, playerX, playerY);
            break;

        case EnemyState::Dead:
            updateDead(deltaTime, playerX, playerY);
            break;

        default:
            changeState(EnemyState::Idle);
            break;
    }
}

void Enemy::updateFlyingType(float deltaTime, float playerX, float playerY) {
    if (state == EnemyState::Dead) {
        updateDead(deltaTime, playerX, playerY);
        return;
    }

    if (state == EnemyState::Hurt) {
        updateHurt(deltaTime, playerX, playerY);
        return;
    }

    float dx = playerX - x;
    float dy = playerY - y;

    if (dx < 0.0f) {
        direction = -1;
    } else if (dx > 0.0f) {
        direction = 1;
    }

    x += direction * speed * deltaTime * 0.8f;

    if (dy < -10.0f) {
        y -= speed * deltaTime * 0.5f;
    } else if (dy > 10.0f) {
        y += speed * deltaTime * 0.5f;
    }
}

// BOSS LOGIC
void Enemy::updateBoss(float deltaTime, float playerX, float playerY) {
    switch (state) {
        case EnemyState::Idle:
            updateBossIdle(deltaTime, playerX, playerY);
            break;
        case EnemyState::Attack:
            updateBossAttack(deltaTime, playerX, playerY);
            break;
        case EnemyState::Hurt:
            updateHurt(deltaTime, playerX, playerY);
            break;
        case EnemyState::Dead:
            updateDead(deltaTime, playerX, playerY);
            break;
        default:
            changeState(EnemyState::Idle);
            break;
    }
}

void Enemy::updateBossIdle(float deltaTime, float playerX, float playerY) {
    bossAttackTimer += deltaTime;

    if (bossAttackTimer >= bossAttackCooldown) {
        bossAttackTimer = 0.0f;
        bossAttackPhase = 0;

        float dist = distanceToPlayer(playerX, playerY);

        if (dist > 350.0f) {
            currentBossAttack = BossAttackType::BurstShot;
        } else {
            currentBossAttack = BossAttackType::DashAttack;
        }

        changeState(EnemyState::Attack);
    }
}

void Enemy::updateBossAttack(float deltaTime, float playerX, float playerY) {
    switch (currentBossAttack) {
        case BossAttackType::BurstShot:
            updateBossBurstShot(deltaTime, playerX, playerY);
            break;
        case BossAttackType::DashAttack:
            updateBossDashAttack(deltaTime, playerX, playerY);
            break;
        default:
            changeState(EnemyState::Idle);
            break;
    }
}

void Enemy::updateBossBurstShot(float deltaTime, float playerX, float playerY) {
    bossAttackTimer += deltaTime;

    if (playerX < x) {
        direction = -1;
    } else {
        direction = 1;
    }

    if (bossAttackPhase == 0) {
        if (bossAttackTimer >= 0.5f) {
            bossAttackTimer = 0.0f;
            bossAttackPhase = 1;
        }
    }
    else if (bossAttackPhase == 1) {
        shotThisFrame = true;
        std::cout << "Boss fires shot 1\n";
        bossAttackTimer = 0.0f;
        bossAttackPhase = 2;
    }
    else if (bossAttackPhase == 2) {
        if (bossAttackTimer >= 0.2f) {
            shotThisFrame = true;
            std::cout << "Boss fires shot 2\n";
            bossAttackTimer = 0.0f;
            bossAttackPhase = 3;
        }
    }
    else if (bossAttackPhase == 3) {
        if (bossAttackTimer >= 0.2f) {
            shotThisFrame = true;
            std::cout << "Boss fires shot 3\n";
            bossAttackTimer = 0.0f;
            bossAttackPhase = 4;
        }
    }
    else if (bossAttackPhase == 4) {
        if (bossAttackTimer >= 0.4f) {
            bossAttackTimer = 0.0f;
            currentBossAttack = BossAttackType::None;
            changeState(EnemyState::Idle);
        }
    }
}

void Enemy::updateBossDashAttack(float deltaTime, float playerX, float playerY) {
    std::cout << "Boss dash attack\n";
    bossAttackTimer += deltaTime;

    if (playerX < x) {
        direction = -1;
    } else {
        direction = 1;
    }

    if (bossAttackPhase == 0) {
        // windup
        if (bossAttackTimer >= 0.4f) {
            bossAttackTimer = 0.0f;
            bossAttackPhase = 1;
        }
    }
    else if (bossAttackPhase == 1) {
        // dash
        x += direction * speed * 3.0f * deltaTime;

        // During dash, boss can hurt player on overlap
        attackHitThisFrame = true;

        if (bossAttackTimer >= 0.5f) {
            bossAttackTimer = 0.0f;
            bossAttackPhase = 2;
        }
    }
    else if (bossAttackPhase == 2) {
        // recovery
        if (bossAttackTimer >= 0.5f) {
            bossAttackTimer = 0.0f;
            currentBossAttack = BossAttackType::None;
            changeState(EnemyState::Idle);
        }
    }
}

// HELPERS
EnemyType stringToEnemyType(const std::string& type) {
    if (type == "Patrol") return EnemyType::Patrol;
    if (type == "Shooter") return EnemyType::Shooter;
    if (type == "Flying") return EnemyType::Flying;
    if (type == "Boss") return EnemyType::Boss;
    return EnemyType::Patrol;
}

const char* enemyTexturePath(EnemyType type) {
    switch (type) {
        case EnemyType::Boss:
            return "../assets/boss1.png";
        case EnemyType::Shooter:
            return "../assets/enemy.png";
        case EnemyType::Flying:
            return "../assets/enemy.png";
        case EnemyType::Patrol:
        default:
            return "../assets/enemy.png";
    }
}

void Enemy::render(SDL_Renderer* renderer, const SDL_FRect& camera) {
    if (!texture) {
        return;
    }

    if (state == EnemyState::Dead) {
        return;
    }

    SDL_FRect screenRect = dstRect;
    screenRect.x = x - camera.x;
    screenRect.y = y - camera.y;

    SDL_RenderTexture(renderer, texture, nullptr, &screenRect);
}

void Enemy::clean() {
    if (texture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
}

SDL_FRect Enemy::getBounds() const {
    return dstRect;
}

EnemyState Enemy::getState() const {
    return state;
}

bool Enemy::isDead() const {
    return state == EnemyState::Dead;
}

bool Enemy::didAttackHit() const {
    return attackHitThisFrame;
}

void Enemy::resetAttackHit() {
    attackHitThisFrame = false;
}

float Enemy::distanceToPlayer(float playerX, float playerY) const {
    float dx = playerX - x;
    float dy = playerY - y;
    return std::sqrt(dx * dx + dy * dy);
}

void Enemy::applyGravity(float deltaTime, const Map& map) {
    if (type == EnemyType::Flying) {
        return;
    }

    velocityY += gravity * deltaTime;
    y += velocityY * deltaTime;

    grounded = false;

    SDL_FRect enemyRect = {x, y, dstRect.w, dstRect.h};

    for (const SDL_FRect& collider : map.getColliders()) {
        bool overlaps =
            enemyRect.x < collider.x + collider.w &&
            enemyRect.x + enemyRect.w > collider.x &&
            enemyRect.y < collider.y + collider.h &&
            enemyRect.y + enemyRect.h > collider.y;

        if (overlaps) {
            float previousBottom = (y - velocityY * deltaTime) + dstRect.h;
            float colliderTop = collider.y;

            if (previousBottom <= colliderTop + 5.0f && velocityY >= 0.0f) {
                y = collider.y - dstRect.h;
                velocityY = 0.0f;
                grounded = true;
                enemyRect.y = y;
            }
        }
    }

    // Prevent enemies leaving the map
    float mapPixelHeight = static_cast<float>(map.getHeight() * map.getTileHeight());

    if (y > mapPixelHeight - dstRect.h) {
        y = mapPixelHeight - dstRect.h;
        velocityY = 0.0f;
        grounded = true;
    }
}

void Enemy::updatePatrolAttack(float deltaTime, float playerX, float playerY) {
    float dist = distanceToPlayer(playerX, playerY);

    attackTimer += deltaTime;

    if (!damageAppliedThisAttack && attackTimer >= attackDuration * 0.5f) {
        damageAppliedThisAttack = true;
        attackHitThisFrame = true;
        std::cout << "Patrol enemy melee hit\n";
    }

    if (attackTimer >= attackDuration) {
        if (dist <= attackRange) {
            changeState(EnemyState::Attack);
        } else if (dist <= chaseRange) {
            changeState(EnemyState::Chase);
        } else {
            changeState(EnemyState::Patrol);
        }
    }
}

void Enemy::updateShooterAttack(float deltaTime, float playerX, float playerY) {
    attackTimer += deltaTime;

    if (playerX < x) {
        direction = -1;
    } else {
        direction = 1;
    }

    if (!damageAppliedThisAttack && attackTimer >= 0.5f) {
        damageAppliedThisAttack = true;
        shotThisFrame = true;
        std::cout << "Shooter enemy fires projectile\n";
    }

    if (attackTimer >= 1.0f) {
        float dist = distanceToPlayer(playerX, playerY);

        if (dist <= chaseRange) {
            changeState(EnemyState::Idle);
        } else {
            changeState(EnemyState::Idle);
        }
    }
}

void Enemy::updateShooterIdle(float deltaTime, float playerX, float playerY) {
    float dist = distanceToPlayer(playerX, playerY);

    if (dist <= chaseRange) {
        changeState(EnemyState::Attack);
    }
}

void Enemy::updateFlyingAttack(float deltaTime, float playerX, float playerY) {
    float dx = playerX - x;
    float dy = playerY - y;

    if (dx < 0.0f) direction = -1;
    else direction = 1;

    x += direction * speed * 1.2f * deltaTime;

    if (dy < -10.0f) y -= speed * deltaTime * 0.8f;
    else if (dy > 10.0f) y += speed * deltaTime * 0.8f;

    attackTimer += deltaTime;

    if (!damageAppliedThisAttack && attackTimer >= 0.3f) {
        damageAppliedThisAttack = true;
        attackHitThisFrame = true;
        std::cout << "Flying enemy dive hit\n";
    }

    if (attackTimer >= 0.7f) {
        changeState(EnemyState::Chase);
    }
}

bool Enemy::didShootThisFrame() const {
    return shotThisFrame;
}

void Enemy::resetShotThisFrame() {
    shotThisFrame = false;
}