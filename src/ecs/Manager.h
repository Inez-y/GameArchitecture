#ifndef LASTCARRIAGE_MANAGER_H
#define LASTCARRIAGE_MANAGER_H
#pragma once

#include <vector>
#include <memory>
#include <algorithm>
#include "Entity.h"

class Manager {
private:
    std::vector<std::unique_ptr<Entity>> entities;

public:
    Entity& addEntity() {
        Entity* e = new Entity();
        std::unique_ptr<Entity> ptr{e};
        entities.emplace_back(std::move(ptr));
        return *e;
    }

    void update(float dt) {
        for (auto& e : entities) e->update(dt);
    }

    void draw() {
        for (auto& e : entities) e->draw();
    }

    void refresh() {
        entities.erase(
            std::remove_if(entities.begin(), entities.end(),
                [](const std::unique_ptr<Entity>& e) { return !e->isActive(); }),
                entities.end()
                );
    }

    std::vector<std::unique_ptr<Entity>>& getEntities() {
        return entities;
    }
};
#endif //LASTCARRIAGE_MANAGER_H