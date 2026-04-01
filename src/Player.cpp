#include "Player.h"
#include <SDL3_image/SDL_image.h>
#include <iostream>

Player::Player()
    : texture(nullptr),
      x(100.0f),
      y(100.0f),
      speed(200.0f),
      moveX(0.0f),
      moveY(0.0f),
      facingDirection(1),
      hp(10),
      maxHP(10),
      velocityY(0.0f),
      gravity(900.0f),
      jumpStrength(450.0f),
      grounded(false),
      ammo(6),
      maxAmmo(6),
      reloading(false),
      reloadTimer(0.0f),
      reloadDuration(1.2f) {
    dstRect = {x, y, 32.0f, 32.0f};
}

bool Player::init(SDL_Renderer* renderer, const char* texturePath, float startX, float startY) {
    texture = IMG_LoadTexture(renderer, texturePath);
    if (!texture) {
        std::cout << "Failed to load player texture: " << SDL_GetError() << std::endl;
        return false;
    }

    x = startX;
    y = startY;

    float texW = 0.0f;
    float texH = 0.0f;
    SDL_GetTextureSize(texture, &texW, &texH);

    dstRect.x = x;
    dstRect.y = y;
    dstRect.w = texW;
    dstRect.h = texH;

    return true;
}

void Player::handleInput(const bool* keyStates) {
    moveX = 0.0f;

    if (keyStates[SDL_SCANCODE_A] || keyStates[SDL_SCANCODE_LEFT]) {
        moveX = -1.0f;
        facingDirection = -1;
    }
    if (keyStates[SDL_SCANCODE_D] || keyStates[SDL_SCANCODE_RIGHT]) {
        moveX = 1.0f;
        facingDirection = 1;
    }

    if ((keyStates[SDL_SCANCODE_W] || keyStates[SDL_SCANCODE_UP]) && grounded) {
        velocityY = -jumpStrength;
        grounded = false;
    }
}

void Player::update(float deltaTime, const Map& map) {
    // Horizontal movement
    x += moveX * speed * deltaTime;

    // Gravity
    velocityY += gravity * deltaTime;
    y += velocityY * deltaTime;

    grounded = false;

    // Very simple collision against map colliders
    SDL_FRect playerRect = {x, y, dstRect.w, dstRect.h};

    for (const SDL_FRect& collider : map.getColliders()) {
        bool overlaps =
            playerRect.x < collider.x + collider.w &&
            playerRect.x + playerRect.w > collider.x &&
            playerRect.y < collider.y + collider.h &&
            playerRect.y + playerRect.h > collider.y;

        if (overlaps) {
            // Only handle landing from above for now
            float previousBottom = (y - velocityY * deltaTime) + dstRect.h;
            float colliderTop = collider.y;

            if (previousBottom <= colliderTop + 5.0f && velocityY >= 0.0f) {
                y = collider.y - dstRect.h;
                velocityY = 0.0f;
                grounded = true;
                playerRect.y = y;
            }
        }
    }

    // Clamp inside map horizontally
    float mapPixelWidth = static_cast<float>(map.getWidth() * map.getTileWidth());
    float mapPixelHeight = static_cast<float>(map.getHeight() * map.getTileHeight());

    if (x < 0.0f) x = 0.0f;
    if (x > mapPixelWidth - dstRect.w) x = mapPixelWidth - dstRect.w;

    // Prevent falling forever
    if (y > mapPixelHeight - dstRect.h) {
        y = mapPixelHeight - dstRect.h;
        velocityY = 0.0f;
        grounded = true;
    }

    // Reload timer
    if (reloading) {
        reloadTimer += deltaTime;
        if (reloadTimer >= reloadDuration) {
            ammo = maxAmmo;
            reloading = false;
            reloadTimer = 0.0f;
        }
    }

    dstRect.x = x;
    dstRect.y = y;
}

void Player::render(SDL_Renderer* renderer, const SDL_FRect& camera) {
    if (!texture) {
        return;
    }

    SDL_FRect screenRect = dstRect;
    screenRect.x -= camera.x;
    screenRect.y -= camera.y;


    SDL_RenderTexture(renderer, texture, nullptr, &screenRect);
}

void Player::clean() {
    if (texture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
}

// For camera
float Player::getX() const {
    return x;
}

float Player::getY() const {
    return y;
}

float Player::getWidth() const {
    return dstRect.w;
}

float Player::getHeight() const {
    return dstRect.h;
}

int Player::getFacingDirection() const {
    return facingDirection;
}

void Player::setPosition(float newX, float newY) {
    x = newX;
    y = newY;
    dstRect.x = x;
    dstRect.y = y;
}

int Player::getHP() const {
    return hp;
}

void Player::takeDamage(int damage) {
    hp -= damage;
    if (hp < 0) {
        hp = 0;
    }
}

void Player::heal(int amount) {
    hp += amount;
    if (hp > maxHP) {
        hp = maxHP;
    }
}

void Player::startReload() {
    if (reloading) {
        return;
    }

    if (ammo == maxAmmo) {
        return;
    }

    reloading = true;
    reloadTimer = 0.0f;
}

bool Player::isReloading() const {
    return reloading;
}

bool Player::isGrounded() const {
    return grounded;
}

int Player::getAmmo() const {
    return ammo;
}

int Player::getMaxAmmo() const {
    return maxAmmo;
}

bool Player::canShoot() const {
    return !reloading && ammo > 0;
}

void Player::consumeAmmo() {
    if (ammo > 0) {
        ammo--;
    }
}
