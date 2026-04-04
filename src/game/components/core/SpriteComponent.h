// Assures ECS components has entity pointer access
#ifndef LASTCARRIAGE_SPRITECOMPONENT_H
#define LASTCARRIAGE_SPRITECOMPONENT_H
#pragma once

#include "../../../engine/ecs/Component.h"
#include "TransformComponent.h"
#include <SDL3/SDL.h>
#include <iostream>

class SpriteComponent : public Component {
public:
    explicit SpriteComponent(SDL_Texture* tex)
        : texture(tex) {}

    void init() override {
        if (entity->hasComponent<TransformComponent>()) {
            transform = &entity->getComponent<TransformComponent>();
        }
    }

    SDL_Texture* getTexture() const {
        return texture;
    }

    void setTexture(SDL_Texture* tex) {
        texture = tex;
    }

    void draw(SDL_Renderer* renderer, const SDL_FRect& camera) {
        if (!renderer || !texture || !transform) {
            return;
        }

        SDL_FRect dstRect;
        dstRect.x = transform->x - camera.x;
        dstRect.y = transform->y - camera.y;
        dstRect.w = transform->w;
        dstRect.h = transform->h;

        SDL_RenderTexture(renderer, texture, nullptr, &dstRect);
    }

private:
    SDL_Texture* texture = nullptr;
    TransformComponent* transform = nullptr;
};

#endif //LASTCARRIAGE_SPRITECOMPONENT_H