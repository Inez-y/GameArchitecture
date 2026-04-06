#ifndef LASTCARRIAGE_ISCENE_H
#define LASTCARRIAGE_ISCENE_H

#pragma once

#include <SDL3/SDL.h>

class IScene {
public:
    virtual ~IScene() = default;

    virtual bool init() = 0;
    virtual bool handleEvents() = 0; // false = quit
    virtual void update(float dt) = 0;
    virtual void render() = 0;
    virtual void clean() = 0;
};

#endif // LASTCARRIAGE_ISCENE_HSCENE_H