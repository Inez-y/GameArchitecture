#ifndef LASTCARRIAGE_BULLETCOMPONENT_H
#define LASTCARRIAGE_BULLETCOMPONENT_H
#pragma once

#include "../ecs/Component.h"

enum class BulletOwner {
    Player,
    Enemy
};

class BulletComponent : public Component {
public:
    BulletOwner owner;
    float speed;
    int direction;
    int damage;
    bool active;

    BulletComponent(BulletOwner owner = BulletOwner::Player,
                    float speed = 300.0f,
                    int direction = 1,
                    int damage = 1)
        : owner(owner),
          speed(speed),
          direction(direction),
          damage(damage),
          active(true) {}

    BulletOwner getOwner() const {
        return owner;
    }

    int getDirection() const {
        return direction;
    }

    int getDamage() const {
        return damage;
    }

    float getSpeed() const {
        return speed;
    }

    bool isActive() const {
        return active;
    }

    void deactivate() {
        active = false;
    }
};
#endif //LASTCARRIAGE_BULLETCOMPONENT_H