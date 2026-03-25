#include "Player.h"
#include <SDL3_image/SDL_image.h>
#include <iostream>

Player::Player()
    : texture(nullptr),
      x(100.0f),
      y(100.0f),
      speed(200.0f),
      moveX(0.0f),
      moveY(0.0f) {
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
    float dx = 0.0f;
    float dy = 0.0f;

    // WASD for test
    if (keyStates[SDL_SCANCODE_W] || keyStates[SDL_SCANCODE_UP]) {
        dy -= 1.0f;
    }
    if (keyStates[SDL_SCANCODE_S] || keyStates[SDL_SCANCODE_DOWN]) {
        dy += 1.0f;
    }
    if (keyStates[SDL_SCANCODE_A] || keyStates[SDL_SCANCODE_LEFT]) {
        dx -= 1.0f;
    }
    if (keyStates[SDL_SCANCODE_D] || keyStates[SDL_SCANCODE_RIGHT]) {
        dx += 1.0f;
    }

    // Attack spacebar
    // Jump Lshift
    x += dx * speed / 60.0f;
    y += dy * speed / 60.0f;

    dstRect.x = x;
    dstRect.y = y;
}

void Player::update(float deltaTime, float mapPixelWidth, float mapPixelHeight) {
    x += moveX * speed * deltaTime;
    y += moveY * speed * deltaTime;

    if (x < 0.0f) {
        x = 0.0f;
    }
    if (y < 0.0f) {
        y = 0.0f;
    }

    if (x > mapPixelWidth - dstRect.w) {
        x = mapPixelWidth - dstRect.w;
    }
    if (y > mapPixelHeight - dstRect.h) {
        y = mapPixelHeight - dstRect.h;
    }

    if (x < 0.0f) {
        x = 0.0f;
    }
    if (y < 0.0f) {
        y = 0.0f;
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