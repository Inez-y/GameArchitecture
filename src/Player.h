#ifndef LASTCARRIAGE_PLAYER_H
#define LASTCARRIAGE_PLAYER_H

#pragma once

#include <SDL3/SDL.h>
#include "Map.h"

class Player {
public:
    Player();

    bool init(SDL_Renderer* renderer, const char* texturePath, float x, float y);
    void handleInput(const bool* keyStates);
    void update(float deltaTime, const Map& map);
    void render(SDL_Renderer* renderer, const SDL_FRect& camera);
    void clean();

    // For camera
    float getX() const;
    float getY() const;
    float getWidth() const;
    float getHeight() const;

    // For attack
    int getFacingDirection() const;

    void setPosition(float newX, float newY);

    int getHP() const;
    void takeDamage(int damage);
    void heal(int amount);

    bool isGrounded() const;
    bool isReloading() const;
    void startReload();
    int getAmmo() const;
    int getMaxAmmo() const;

    bool canShoot() const;
    void consumeAmmo();

private:
    SDL_Texture* texture;
    SDL_FRect dstRect;

    float x;
    float y;
    float speed;

    // For staying inside a map
    float moveX;
    float moveY;

    // For attack
    int facingDirection; // 1 (right) -1 (left)

    int hp;
    int maxHP;

    float velocityY;
    float gravity;
    float jumpStrength;
    bool grounded;

    int ammo;
    int maxAmmo;
    bool reloading;
    float reloadTimer;
    float reloadDuration;
};

#endif //LASTCARRIAGE_PLAYER_H