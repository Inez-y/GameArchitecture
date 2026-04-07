#ifndef LASTCARRIAGE_ANIMATIONCOMPONENT_H
#define LASTCARRIAGE_ANIMATIONCOMPONENT_H

#pragma once

#include "../../../engine/ecs/Component.h"
#include <string>
#include <unordered_map>

struct AnimationClip {
    std::string spritesheetPath;
    int row = 0;
    int frameWidth = 0;
    int frameHeight = 0;
    int frameCount = 1;
    float frameDuration = 0.1f;
    bool loop = true;
};

class AnimationComponent : public Component {
public:
    AnimationComponent() = default;

    void addClip(const std::string& name, const AnimationClip& clip) {
        clips[name] = clip;
    }

    void play(const std::string& name, bool restart = false) {
        if (!restart && currentClip == name) {
            return;
        }

        auto it = clips.find(name);
        if (it == clips.end()) {
            return;
        }

        currentClip = name;
        currentFrame = 0;
        timer = 0.0f;
        finished = false;
    }

    bool hasClip(const std::string& name) const {
        return clips.find(name) != clips.end();
    }

    const AnimationClip* getCurrentClip() const {
        auto it = clips.find(currentClip);
        if (it == clips.end()) {
            return nullptr;
        }
        return &it->second;
    }

    const std::string& getCurrentClipName() const {
        return currentClip;
    }

    int currentFrame = 0;
    float timer = 0.0f;
    bool finished = false;
    bool flipX = false;

private:
    std::unordered_map<std::string, AnimationClip> clips;
    std::string currentClip;
};

#endif // LASTCARRIAGE_ANIMATIONCOMPONENT_H