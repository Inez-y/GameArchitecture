#ifndef LASTCARRIAGE_INPUTCOMPONENT_H
#define LASTCARRIAGE_INPUTCOMPONENT_H

#pragma once

#include "../../../engine/ecs/Component.h"
#include "PhysicsComponent.h"
#include "../gameplay/WeaponComponent.h"

#include <SDL3/SDL.h>
#include <cstdint>

class InputComponent : public Component {
public:
    PhysicsComponent* physics;
    WeaponComponent* weapon;

    Uint64 lastLeftTapTicks;
    Uint64 lastRightTapTicks;
    Uint64 dashEndTicks;
    int dashDirection;

    float dashMoveMultiplier;
    Uint64 doubleTapWindowMs;
    Uint64 dashDurationMs;

    bool previousLeftDown;
    bool previousRightDown;

    InputComponent()
        : physics(nullptr),
          weapon(nullptr),
          lastLeftTapTicks(0),
          lastRightTapTicks(0),
          dashEndTicks(0),
          dashDirection(0),
          dashMoveMultiplier(2.6f),
          doubleTapWindowMs(250),
          dashDurationMs(160),
          previousLeftDown(false),
          previousRightDown(false) {}

    void init() override {
        physics = &entity->getComponent<PhysicsComponent>();
        weapon = &entity->getComponent<WeaponComponent>();
    }

    bool isDashing() const {
        return SDL_GetTicks() < dashEndTicks;
    }

    int getDashDirection() const {
        return dashDirection;
    }

    void handleInput(const bool* keyStates) {
        if (!physics || !weapon || !keyStates) {
            return;
        }

        const bool leftDown = keyStates[SDL_SCANCODE_A] || keyStates[SDL_SCANCODE_LEFT];
        const bool rightDown = keyStates[SDL_SCANCODE_D] || keyStates[SDL_SCANCODE_RIGHT];
        const bool jumpDown = keyStates[SDL_SCANCODE_W] || keyStates[SDL_SCANCODE_UP];
        const Uint64 now = SDL_GetTicks();

        if (leftDown && !previousLeftDown) {
            if (now - lastLeftTapTicks <= doubleTapWindowMs) {
                dashDirection = -1;
                dashEndTicks = now + dashDurationMs;
            }
            lastLeftTapTicks = now;
        }

        if (rightDown && !previousRightDown) {
            if (now - lastRightTapTicks <= doubleTapWindowMs) {
                dashDirection = 1;
                dashEndTicks = now + dashDurationMs;
            }
            lastRightTapTicks = now;
        }

        physics->moveX = 0.0f;

        if (isDashing()) {
            physics->moveX = static_cast<float>(dashDirection) * dashMoveMultiplier;
            weapon->facingDirection = dashDirection;
        } else {
            dashDirection = 0;

            if (leftDown) {
                physics->moveX = -1.0f;
                weapon->facingDirection = -1;
            }

            if (rightDown) {
                physics->moveX = 1.0f;
                weapon->facingDirection = 1;
            }
        }

        if (jumpDown) {
            physics->jump();
        }

        previousLeftDown = leftDown;
        previousRightDown = rightDown;
    }
};

#endif //LASTCARRIAGE_INPUTCOMPONENT_H
