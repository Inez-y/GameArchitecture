#include "AudioSystem.h"
#include "BgmPlayer.h"

bool AudioSystem::init() {
    initialized = true;
    currentTrack.clear();
    currentVolume = 1.0f;
    return true;
}

void AudioSystem::update(float dt) {
    (void)dt;
    // later:
    // - check scene state
    // - fade music
    // - trigger SFX
}

void AudioSystem::clean() {
    BgmPlayer::stop();
    currentTrack.clear();
    initialized = false;
}

bool AudioSystem::playBgm(const std::string& path, float volume) {
    if (!initialized || path.empty()) {
        return false;
    }

    if (currentTrack == path) {
        BgmPlayer::setVolume(volume);
        currentVolume = volume;
        return true;
    }

    if (!BgmPlayer::playLoop(path.c_str(), volume)) {
        return false;
    }

    currentTrack = path;
    currentVolume = volume;
    return true;
}

void AudioSystem::stopBgm() {
    BgmPlayer::stop();
    currentTrack.clear();
}

void AudioSystem::setBgmVolume(float volume) {
    currentVolume = volume;
    BgmPlayer::setVolume(volume);
}

const std::string& AudioSystem::getCurrentTrack() const {
    return currentTrack;
}