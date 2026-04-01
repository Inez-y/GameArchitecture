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
};

#endif //LASTCARRIAGE_PLAYER_H