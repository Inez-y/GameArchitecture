#ifndef LASTCARRIAGE_PLAYER_H
#define LASTCARRIAGE_PLAYER_H

#pragma once

#include <SDL3/SDL.h>

class Player {
public:
    Player();

    bool init(SDL_Renderer* renderer, const char* texturePath, float x, float y);
    void handleInput(const bool* keyStates);
    void update(float deltaTime, float mapPixelWidth, float mapPixelHeight);
    void render(SDL_Renderer* renderer, const SDL_FRect& camera);
    void clean();

    // For camera
    float getX() const;
    float getY() const;
    float getWidth() const;
    float getHeight() const;

private:
    SDL_Texture* texture;
    SDL_FRect dstRect;

    float x;
    float y;
    float speed;

    // For staying inside a map
    float moveX;
    float moveY;
};

#endif //LASTCARRIAGE_PLAYER_H