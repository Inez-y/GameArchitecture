#ifndef LASTCARRIAGE_COMPONENT_H
#define LASTCARRIAGE_COMPONENT_H

#pragma once

class Entity;

class Component {
public:
    Entity* entity = nullptr;

    virtual void init() {}
    virtual void update(float dt) {}
    virtual void draw() {}
    virtual ~Component() = default;
};
#endif //LASTCARRIAGE_COMPONENT_H