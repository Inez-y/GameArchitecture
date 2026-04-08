#ifndef LASTCARRIAGE_WEAPONCOMPONENT_H
#define LASTCARRIAGE_WEAPONCOMPONENT_H

#pragma once

#include "../../../engine/ecs/Component.h"

class WeaponComponent : public Component {
public:
    int facingDirection;
    int ammo;
    int maxAmmo;

    bool reloading;
    float reloadTimer;
    float reloadDuration;

    bool attacking;
    float attackTimer;
    float attackDuration;

    WeaponComponent(int maxAmmo = 6, float reloadDuration = 1.2f)
        : facingDirection(1), // -1(left) 1(right)
          ammo(maxAmmo),
          maxAmmo(maxAmmo),
          reloading(false),
          reloadTimer(0.0f),
          reloadDuration(reloadDuration),
          attacking(false),
          attackTimer(0.0f),
          attackDuration(0.35f) {}

    void update(float deltaTime) {
        if (reloading) {
            reloadTimer += deltaTime;

            if (reloadTimer >= reloadDuration) {
                ammo = maxAmmo;
                reloading = false;
                reloadTimer = 0.0f;
            }
        }

        if (attacking) {
            attackTimer += deltaTime;

            if (attackTimer >= attackDuration) {
                attacking = false;
                attackTimer = 0.0f;
            }
        }
    }

    bool canShoot() const {
        return !reloading && !attacking && ammo > 0;
    }

    void consumeAmmo() {
        if (ammo > 0 && !attacking) {
            ammo--;
            startAttack();
        }
    }

    void startAttack() {
        if (attacking) {
            return;
        }

        attacking = true;
        attackTimer = 0.0f;
    }

    void startReload() {
        if (reloading || attacking) {
            return;
        }

        if (ammo == maxAmmo) {
            return;
        }

        reloading = true;
        reloadTimer = 0.0f;
    }

    bool isReloading() const {
        return reloading;
    }

    bool isAttacking() const {
        return attacking;
    }

    int getAmmo() const {
        return ammo;
    }

    int getMaxAmmo() const {
        return maxAmmo;
    }

    int getFacingDirection() const {
        return facingDirection;
    }
};

#endif // LASTCARRIAGE_WEAPONCOMPONENT_H