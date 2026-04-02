// Assures ECS components has entity pointer access
#ifndef LASTCARRIAGE_SPRITECOMPONENT_H
#define LASTCARRIAGE_SPRITECOMPONENT_H

#pragma once

#include "../ecs/Component.h"
#include "TransformComponent.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <iostream>

class SpriteComponent : public Component {
public:
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    TransformComponent* transform;

    SpriteComponent(SDL_Renderer* renderer, const char* texturePath)
        : renderer(renderer), texture(nullptr), transform(nullptr) {
        texture = IMG_LoadTexture(renderer, texturePath);
        if (!texture) {
            std::cout << "Failed to load sprite texture: " << SDL_GetError() << std::endl;
        }
    }

    ~SpriteComponent() override {
        if (texture) {
            SDL_DestroyTexture(texture);
            texture = nullptr;
        }
    }

    void init() override {
        transform = &entity->getComponent<TransformComponent>();
    }

    void draw(const SDL_FRect& camera) {
        if (!texture || !transform) {
            return;
        }

        SDL_FRect dstRect;
        dstRect.x = transform->x - camera.x;
        dstRect.y = transform->y - camera.y;
        dstRect.w = transform->w;
        dstRect.h = transform->h;

        SDL_RenderTexture(renderer, texture, nullptr, &dstRect);
    }
};

#endif //LASTCARRIAGE_SPRITECOMPONENT_H