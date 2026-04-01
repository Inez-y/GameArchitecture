#ifndef LASTCARRIAGE_ENEMYBULLET_H
#define LASTCARRIAGE_ENEMYBULLET_H

#pragma once

#include <SDL3/SDL.h>

class EnemyBullet {
public:
    EnemyBullet();

    void init(float startX, float startY, int direction);
    void update(float deltaTime);
    void render(SDL_Renderer* renderer, const SDL_FRect& camera);

    SDL_FRect getBounds() const;
    bool isActive() const;
    void deactivate();

private:
    SDL_FRect rect;
    float speed;
    int direction;
    bool active;
};

#endif //LASTCARRIAGE_ENEMYBULLET_H