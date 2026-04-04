#ifndef LASTCARRIAGE_DOORCOMPONENT_H
#define LASTCARRIAGE_DOORCOMPONENT_H

#pragma once

#include "../ecs/Component.h"
#include <string>

class DoorComponent : public Component {
public:
    DoorComponent(const std::string& targetMap = "", const std::string& targetSpawn = "default")
        : targetMap(targetMap), targetSpawn(targetSpawn), active(true) {}

    const std::string& getTargetMap() const {
        return targetMap;
    }

    const std::string& getTargetSpawn() const {
        return targetSpawn;
    }

    bool isActive() const {
        return active;
    }

    void setTargetMap(const std::string& value) {
        targetMap = value;
    }

    void setTargetSpawn(const std::string& value) {
        targetSpawn = value;
    }

    void setActive(bool value) {
        active = value;
    }

private:
    std::string targetMap;
    std::string targetSpawn;
    bool active = true;
};

#endif //LASTCARRIAGE_DOORCOMPONENT_H