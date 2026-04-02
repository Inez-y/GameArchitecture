#ifndef LASTCARRIAGE_PHYSICSCOMPONENT_H
#define LASTCARRIAGE_PHYSICSCOMPONENT_H

#pragma once

#include "../ecs/Component.h"
#include "TransformComponent.h"
#include "../map/Map.h"

class PhysicsComponent : public Component {
public:
    float speed;
    float moveX;

    float velocityY;
    float gravity;
    float jumpStrength;
    bool grounded;

    TransformComponent* transform;

    PhysicsComponent(float speed = 200.0f)
        : speed(speed),
          moveX(0.0f),
          velocityY(0.0f),
          gravity(900.0f),
          jumpStrength(450.0f),
          grounded(false),
          transform(nullptr) {}

    void init() override {
        transform = &entity->getComponent<TransformComponent>();
    }

    void jump() {
        if (grounded) {
            velocityY = -jumpStrength;
            grounded = false;
        }
    }

    void update(float deltaTime, const Map& map) {
        if (!transform) {
            return;
        }

        transform->x += moveX * speed * deltaTime;

        velocityY += gravity * deltaTime;
        transform->y += velocityY * deltaTime;

        grounded = false;

        SDL_FRect playerRect = transform->getRect();

        for (const SDL_FRect& collider : map.getColliders()) {
            bool overlaps =
                playerRect.x < collider.x + collider.w &&
                playerRect.x + playerRect.w > collider.x &&
                playerRect.y < collider.y + collider.h &&
                playerRect.y + playerRect.h > collider.y;

            if (overlaps) {
                float previousBottom = (transform->y - velocityY * deltaTime) + transform->h;
                float colliderTop = collider.y;

                if (previousBottom <= colliderTop + 5.0f && velocityY >= 0.0f) {
                    transform->y = collider.y - transform->h;
                    velocityY = 0.0f;
                    grounded = true;
                    playerRect.y = transform->y;
                }
            }
        }

        float mapPixelWidth = static_cast<float>(map.getWidth() * map.getTileWidth());
        float mapPixelHeight = static_cast<float>(map.getHeight() * map.getTileHeight());

        if (transform->x < 0.0f) {
            transform->x = 0.0f;
        }

        if (transform->x > mapPixelWidth - transform->w) {
            transform->x = mapPixelWidth - transform->w;
        }

        if (transform->y > mapPixelHeight - transform->h) {
            transform->y = mapPixelHeight - transform->h;
            velocityY = 0.0f;
            grounded = true;
        }
    }

    bool isGrounded() const {
        return grounded;
    }
};

#endif //LASTCARRIAGE_PHYSICSCOMPONENT_H