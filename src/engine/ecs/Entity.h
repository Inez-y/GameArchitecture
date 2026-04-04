#ifndef LASTCARRIAGE_ENTITY_H
#define LASTCARRIAGE_ENTITY_H

#pragma once

#include <vector>
#include <memory>
#include <type_traits>
#include "Component.h"

class Entity {
private:
    std::vector<std::unique_ptr<Component>> components;
    bool active = true;

public:
    void update(float dt) {
        for (auto& c : components) c->update(dt);
    }

    void draw() {
        for (auto& c : components) c->draw();
    }

    bool isActive() const { return active; }
    void destroy() { active = false; }

    template<typename T, typename... TArgs>
    T& addComponent(TArgs&&... args) {
        T* c(new T(std::forward<TArgs>(args)...));
        c->entity = this;
        std::unique_ptr<Component> ptr{c};
        components.emplace_back(std::move(ptr));
        c->init();
        return *c;
    }

    template<typename T>
    T& getComponent() const {
        for (auto& c : components) {
            if (auto ptr = dynamic_cast<T*>(c.get())) {
                return *ptr;
            }
        }
        throw std::runtime_error("Component not found");
    }

    template<typename T>
    bool hasComponent() const {
        for (auto& c : components) {
            if (dynamic_cast<T*>(c.get())) return true;
        }
        return false;
    }
};
#endif //LASTCARRIAGE_ENTITY_H