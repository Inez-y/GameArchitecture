#ifndef LASTCARRIAGE_DOORCOMPONENT_H
#define LASTCARRIAGE_DOORCOMPONENT_H

#pragma once

#include "../ecs/Component.h"
#include <string>

class DoorComponent : public Component {
public:
    std::string targetMap;
    bool active;

    DoorComponent(const std::string& target = "")
        : targetMap(target), active(true) {}

    const std::string& getTargetMap() const {
        return targetMap;
    }

    bool isActive() const {
        return active;
    }

    void setActive(bool value) {
        active = value;
    }
};

#endif //LASTCARRIAGE_DOORCOMPONENT_H