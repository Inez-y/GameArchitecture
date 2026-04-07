#ifndef LASTCARRIAGE_PHYSICSCOMPONENT_H
#define LASTCARRIAGE_PHYSICSCOMPONENT_H
#pragma once

#include "../../../engine/ecs/Component.h"
#include "../../../engine/ecs/Entity.h"
#include "../../map/runtime/Map.h"
#include "TransformComponent.h"
#include <SDL3/SDL.h>

class PhysicsComponent : public Component {
public:
    float speed;
    float moveX;

    float velocityY;
    float gravity;
    float jumpStrength;

    bool grounded;
    bool useGravity;

    TransformComponent* transform;

    PhysicsComponent(float moveSpeed = 100.0f,
                     float gravityValue = 900.0f,
                     float jumpValue = 450.0f,
                     bool gravityEnabled = true)
        : speed(moveSpeed),
          moveX(0.0f),
          velocityY(0.0f),
          gravity(gravityValue),
          jumpStrength(jumpValue),
          grounded(false),
          useGravity(gravityEnabled),
          transform(nullptr) {}

    void init() override {
        transform = &entity->getComponent<TransformComponent>();
    }

    void setMoveX(float value) {
        moveX = value;
    }

    float getMoveX() const {
        return moveX;
    }

    void setSpeed(float value) {
        speed = value;
    }

    float getSpeed() const {
        return speed;
    }

    void setUseGravity(bool enabled) {
        useGravity = enabled;
    }
    float getVelocityY() const {
        return velocityY;
    }

    bool isUsingGravity() const {
        return useGravity;
    }

    bool isGrounded() const {
        return grounded;
    }

    void jump() {
        if (grounded && useGravity) {
            velocityY = -jumpStrength;
            grounded = false;
        }
    }

    void update(float deltaTime, const Map& map) {
        if (!transform) {
            return;
        }

        // Horizontal movement
        transform->x += moveX * speed * deltaTime;

        if (!useGravity) {
            return;
        }

        // Vertical movement
        velocityY += gravity * deltaTime;
        transform->y += velocityY * deltaTime;

        grounded = false;

        SDL_FRect rect = transform->getRect();

        for (const SDL_FRect& collider : map.getColliders()) {
            bool overlaps =
                rect.x < collider.x + collider.w &&
                rect.x + rect.w > collider.x &&
                rect.y < collider.y + collider.h &&
                rect.y + rect.h > collider.y;

            if (overlaps) {
                float previousBottom = (transform->y - velocityY * deltaTime) + transform->h;
                float colliderTop = collider.y;

                // Simple landing-from-above handling
                if (previousBottom <= colliderTop + 5.0f && velocityY >= 0.0f) {
                    transform->y = collider.y - transform->h;
                    velocityY = 0.0f;
                    grounded = true;
                    rect.y = transform->y;
                }
            }
        }

        // Keep inside bottom of map
        float mapPixelHeight = static_cast<float>(map.getHeight() * map.getTileHeight());
        if (transform->y > mapPixelHeight - transform->h) {
            transform->y = mapPixelHeight - transform->h;
            velocityY = 0.0f;
            grounded = true;
        }

        // Keep inside horizontal map bounds
        float mapPixelWidth = static_cast<float>(map.getWidth() * map.getTileWidth());
        if (transform->x < 0.0f) {
            transform->x = 0.0f;
        }
        if (transform->x > mapPixelWidth - transform->w) {
            transform->x = mapPixelWidth - transform->w;
        }
    }
};

#endif //LASTCARRIAGE_PHYSICSCOMPONENT_H