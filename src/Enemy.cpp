#include "Enemy.h"
#include <SDL3_image/SDL_image.h>
#include <iostream>
#include <cmath>

Enemy::Enemy()
    : texture(nullptr),
      x(0.0f),
      y(0.0f),
      speed(100.0f),
      patrolLeft(0.0f),
      patrolRight(0.0f),
      direction(1),
      state(EnemyState::Patrol),
      idleTimer(0.0f),
      idleDuration(1.0f),
      chaseRange(220.0f),
      stopChaseRange(260.0f),
      attackRange(40.0f),
      attackTimer(0.0f),
      attackDuration(0.6f),
      damageAppliedThisAttack(false),
      hurtTimer(0.0f),
      hurtDuration(0.35f),
      health(3) {
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

    float texW = 0.0f;
    float texH = 0.0f;
    SDL_GetTextureSize(texture, &texW, &texH);

    dstRect = {x, y, texW, texH};

    return true;
}

void Enemy::update(float deltaTime, float playerX, float playerY) {
    // State
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
            updateAttack(deltaTime, playerX, playerY);
            break;
        case EnemyState::Hurt:
            updateHurt(deltaTime, playerX, playerY);
            break;
        case EnemyState::Dead:
            updateDead(deltaTime, playerX, playerY);
            break;
    }

    // Type
    switch (type) {
        case EnemyType::Patrol:
            // Update later
            // updatePatrolType(deltaTime, playerX, playerY);
            break;
        case EnemyType::Shooter:
            // Update later
            updateShooterType(deltaTime, playerX, playerY);
            break;
        case EnemyType::Flying:
            // Update later
            updateFlyingType(deltaTime, playerX, playerY);
            break;
        case EnemyType::Boss:
            // updateBossType(deltaTime, playerX, playerY);
            break;
    }

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
    }
    else if (x >= patrolRight) {
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
    }
    else if (playerX > x) {
        direction = 1;
    }

    x += direction * speed * 1.2f * deltaTime;
}

void Enemy::updateAttack(float deltaTime, float playerX, float playerY) {
    float dist = distanceToPlayer(playerX, playerY);

    attackTimer += deltaTime;

    if (!damageAppliedThisAttack && attackTimer >= attackDuration * 0.5f) {
        damageAppliedThisAttack = true;

        // Later: actually damage the player here
        std::cout << "Enemy attack hit frame\n";
    }

    if (attackTimer >= attackDuration) {
        if (dist <= attackRange) {
            changeState(EnemyState::Attack);
        }
        else if (dist <= chaseRange) {
            changeState(EnemyState::Chase);
        }
        else {
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
        }
        else if (dist <= attackRange) {
            changeState(EnemyState::Attack);
        }
        else if (dist <= chaseRange) {
            changeState(EnemyState::Chase);
        }
        else {
            changeState(EnemyState::Patrol);
        }
    }
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

void Enemy::updateDead(float deltaTime, float playerX, float playerY) {
    // Dead enemies do nothing
    // Update later
}

// Transition helper
void Enemy::changeState(EnemyState newState) {
    if (state == newState) {
        return;
    }

    state = newState;

    if (state == EnemyState::Idle) {
        idleTimer = 0.0f;
    }
    else if (state == EnemyState::Attack) {
        attackTimer = 0.0f;
        damageAppliedThisAttack = false;
    }
    else if (state == EnemyState::Hurt) {
        hurtTimer = 0.0f;
    }
}

// Update by types
void Enemy::updateShooterType(float deltaTime, float playerX, float playerY) {
    float dist = distanceToPlayer(playerX, playerY);

    switch (state) {
        case EnemyState::Idle:
            if (dist < chaseRange) {
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
    float dx = playerX - x;
    float dy = playerY - y;

    if (dx < 0.0f) direction = -1;
    else if (dx > 0.0f) direction = 1;

    x += direction * speed * deltaTime * 0.8f;

    if (dy < -10.0f) y -= speed * deltaTime * 0.5f;
    else if (dy > 10.0f) y += speed * deltaTime * 0.5f;
}

EnemyType stringToEnemyType(const std::string& type) {
    if (type == "Patrol") return EnemyType::Patrol;
    if (type == "Shooter") return EnemyType::Shooter;
    if (type == "Flying") return EnemyType::Flying;
    if (type == "Boss") return EnemyType::Boss;
    return EnemyType::Patrol;
}

const char* enemyTexturePath(EnemyType type) {
    switch (type) {
        // case EnemyType::Patrol: return "assets/enemy_patrol.png";
        // case EnemyType::Shooter: return "assets/enemy_shooter.png";
        case EnemyType::Boss: return "../assets/boss1.png";
        default: return "../assets/enemy.png";
    }
}

void Enemy::render(SDL_Renderer *renderer, const SDL_FRect& camera) {
    if (!texture) { return; }

    if (state == EnemyState::Dead) {
        return;
    }

    SDL_FRect screenRect = dstRect;
    screenRect.x = x;
    screenRect.y = y;

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

float Enemy::distanceToPlayer(float playerX, float playerY) const {
    float dx = playerX - x;
    float dy = playerY - y;
    return std::sqrt(dx * dx + dy * dy);
}