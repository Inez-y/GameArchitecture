#include "EnemyBullet.h"

EnemyBullet::EnemyBullet()
    : speed(300.0f),
      direction(1),
      active(false) {
    rect = {0.0f, 0.0f, 16.0f, 8.0f};
}

void EnemyBullet::init(float startX, float startY, int dir) {
    rect.x = startX;
    rect.y = startY;
    rect.w = 30.0f;
    rect.h = 12.0f;
    direction = dir;
    active = true;
}

void EnemyBullet::update(float deltaTime) {
    if (!active) {
        return;
    }

    rect.x += direction * speed * deltaTime;
}

void EnemyBullet::render(SDL_Renderer* renderer, const SDL_FRect& camera) {
    if (!active) {
        return;
    }
    SDL_FRect screenRect = rect;
    screenRect.x -= camera.x;
    screenRect.y -= camera.y;

    SDL_SetRenderDrawColor(renderer, 255, 192, 203, 255);
    SDL_RenderFillRect(renderer, &screenRect);
}

SDL_FRect EnemyBullet::getBounds() const {
    return rect;
}

bool EnemyBullet::isActive() const {
    return active;
}

void EnemyBullet::deactivate() {
    active = false;
}