#ifndef LASTCARRIAGE_AUDIOSYSTEM_H
#define LASTCARRIAGE_AUDIOSYSTEM_H

#pragma once

#include <string>

class AudioSystem {
public:
    bool init();
    void update(float dt);
    void clean();

    bool playBgm(const std::string& path, float volume = 1.0f);
    void stopBgm();
    void setBgmVolume(float volume);

    const std::string& getCurrentTrack() const;

private:
    std::string currentTrack;
    float currentVolume = 1.0f;
    bool initialized = false;
};

#endif //LASTCARRIAGE_AUDIOSYSTEM_H