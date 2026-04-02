#ifndef LASTCARRIAGE_WEAPONCOMPONENT_H
#define LASTCARRIAGE_WEAPONCOMPONENT_H

#pragma once

#include "../ecs/Component.h"

class WeaponComponent : public Component {
public:
    int facingDirection;
    int ammo;
    int maxAmmo;

    bool reloading;
    float reloadTimer;
    float reloadDuration;

    WeaponComponent(int maxAmmo = 6, float reloadDuration = 1.2f)
        : facingDirection(1),
          ammo(maxAmmo),
          maxAmmo(maxAmmo),
          reloading(false),
          reloadTimer(0.0f),
          reloadDuration(reloadDuration) {}

    void update(float deltaTime) {
        if (reloading) {
            reloadTimer += deltaTime;

            if (reloadTimer >= reloadDuration) {
                ammo = maxAmmo;
                reloading = false;
                reloadTimer = 0.0f;
            }
        }
    }

    bool canShoot() const {
        return !reloading && ammo > 0;
    }

    void consumeAmmo() {
        if (ammo > 0) {
            ammo--;
        }
    }

    void startReload() {
        if (reloading) {
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

#endif //LASTCARRIAGE_WEAPONCOMPONENT_H