// Assures ECS components has entity pointer access
#ifndef LASTCARRIAGE_SPRITECOMPONENT_H
#define LASTCARRIAGE_SPRITECOMPONENT_H

#pragma once

#include "../../../engine/ecs/Component.h"
#include "TransformComponent.h"
#include <SDL3/SDL.h>

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

    void setSourceRect(const SDL_FRect& rect) {
        srcRect = rect;
        useSourceRect = true;
    }

    void clearSourceRect() {
        useSourceRect = false;
    }

    void draw(SDL_Renderer* renderer, const SDL_FRect& camera, bool flipX = false) {
        if (!renderer || !texture || !transform) {
            return;
        }

        SDL_FRect dstRect{
            transform->x - camera.x,
            transform->y - camera.y,
            transform->w,
            transform->h
        };

        SDL_FlipMode flipMode = flipX ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

        if (useSourceRect) {
            SDL_RenderTextureRotated(renderer, texture, &srcRect, &dstRect, 0.0, nullptr, flipMode);
        } else {
            SDL_RenderTextureRotated(renderer, texture, nullptr, &dstRect, 0.0, nullptr, flipMode);
        }
    }

private:
    SDL_Texture* texture = nullptr;
    TransformComponent* transform = nullptr;

    SDL_FRect srcRect{0, 0, 0, 0};
    bool useSourceRect = false;
};

#endif