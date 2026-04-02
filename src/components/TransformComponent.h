#ifndef LASTCARRIAGE_TRANSFROMCOMPONENT_H
#define LASTCARRIAGE_TRANSFROMCOMPONENT_H
#pragma once

#include "../ecs/Component.h"
#include <SDL3/SDL.h>

class TransformComponent : public Component {
public:
    float x;
    float y;
    float w;
    float h;

    TransformComponent(float startX = 0.0f,
                       float startY = 0.0f,
                       float width = 32.0f,
                       float height = 32.0f)
        : x(startX), y(startY), w(width), h(height) {}

    SDL_FRect getRect() const {
        return SDL_FRect{x, y, w, h};
    }

    void setPosition(float newX, float newY) {
        x = newX;
        y = newY;
    }

    float getCenterX() const {
        return x + w * 0.5f;
    }

    float getCenterY() const {
        return y + h * 0.5f;
    }
};
#endif //LASTCARRIAGE_TRANSFROMCOMPONENT_H