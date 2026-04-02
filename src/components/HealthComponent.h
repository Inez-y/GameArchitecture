#ifndef LASTCARRIAGE_HEALTHCOMPONENT_H
#define LASTCARRIAGE_HEALTHCOMPONENT_H

#pragma once

#include "../ecs/Component.h"

class HealthComponent : public Component {
public:
    int hp;
    int maxHP;

    HealthComponent(int maxHP = 10)
        : hp(maxHP), maxHP(maxHP) {}

    void takeDamage(int damage) {
        hp -= damage;
        if (hp < 0) {
            hp = 0;
        }
    }

    void heal(int amount) {
        hp += amount;
        if (hp > maxHP) {
            hp = maxHP;
        }
    }

    bool isDead() const {
        return hp <= 0;
    }

    int getHP() const {
        return hp;
    }

    int getMaxHP() const {
        return maxHP;
    }
};
#endif //LASTCARRIAGE_HEALTHCOMPONENT_H