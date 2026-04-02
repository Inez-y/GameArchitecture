#ifndef LASTCARRIAGE_INPUTCOMPONENT_H
#define LASTCARRIAGE_INPUTCOMPONENT_H

#pragma once

#include "../ecs/Component.h"
#include "PhysicsComponent.h"
#include "WeaponComponent.h"

#include <SDL3/SDL.h>

class InputComponent : public Component {
public:
    PhysicsComponent* physics;
    WeaponComponent* weapon;

    InputComponent()
        : physics(nullptr), weapon(nullptr) {}

    void init() override {
        physics = &entity->getComponent<PhysicsComponent>();
        weapon = &entity->getComponent<WeaponComponent>();
    }

    void handleInput(const bool* keyStates) {
        if (!physics || !weapon) {
            return;
        }

        physics->moveX = 0.0f;

        if (keyStates[SDL_SCANCODE_A] || keyStates[SDL_SCANCODE_LEFT]) {
            physics->moveX = -1.0f;
            weapon->facingDirection = -1;
        }

        if (keyStates[SDL_SCANCODE_D] || keyStates[SDL_SCANCODE_RIGHT]) {
            physics->moveX = 1.0f;
            weapon->facingDirection = 1;
        }

        if (keyStates[SDL_SCANCODE_W] || keyStates[SDL_SCANCODE_UP]) {
            physics->jump();
        }
    }
};

#endif //LASTCARRIAGE_INPUTCOMPONENT_H