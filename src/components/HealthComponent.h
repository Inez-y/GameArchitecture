#ifndef LASTCARRIAGE_HEALTHCOMPONENT_H
#define LASTCARRIAGE_HEALTHCOMPONENT_H

#pragma once

#include "../ecs/Component.h"

class HealthComponent : public Component {
public:
    int hp;
    int maxHP;

    HealthComponent(int maxHealth = 10)
        : hp(maxHealth), maxHP(maxHealth) {}

    int getHP() const {
        return hp;
    }

    int getMaxHP() const {
        return maxHP;
    }

    void setHP(int value) {
        hp = value;
        if (hp < 0) hp = 0;
        if (hp > maxHP) hp = maxHP;
    }

    void setMaxHP(int value) {
        maxHP = value;
        if (maxHP < 1) maxHP = 1;
        if (hp > maxHP) hp = maxHP;
    }

    void takeDamage(int amount) {
        hp -= amount;
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
};

#endif //LASTCARRIAGE_HEALTHCOMPONENT_H