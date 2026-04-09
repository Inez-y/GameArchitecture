#ifndef LASTCARRIAGE_BGMPLAYER_H
#define LASTCARRIAGE_BGMPLAYER_H
#pragma once

class BgmPlayer {
public:
    static bool playLoop(const char* path, float volume = 1.0f);
    static void stop();
    static void setVolume(float volume);
    static bool isPlaying();
};

#endif //LASTCARRIAGE_BGMPLAYER_H