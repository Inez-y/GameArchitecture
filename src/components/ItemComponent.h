#ifndef LASTCARRIAGE_ITEMCOMPONENT_H
#define LASTCARRIAGE_ITEMCOMPONENT_H

#pragma once

#include "../ecs/Component.h"

enum class ItemType {
    Coin,
    Health,
    Key
};

class ItemComponent : public Component {
public:
    ItemType type;
    bool active;

    ItemComponent(ItemType itemType = ItemType::Coin)
        : type(itemType), active(true) {}

    ItemType getType() const {
        return type;
    }

    bool isActive() const {
        return active;
    }

    void pickUp() {
        active = false;
    }
};

#endif //LASTCARRIAGE_ITEMCOMPONENT_H