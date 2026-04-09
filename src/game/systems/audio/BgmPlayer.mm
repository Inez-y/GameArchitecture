/**
 * AVPlayer for mac
 */
#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>

#include "BgmPlayer.h"

namespace {
    AVAudioPlayer* gPlayer = nil;

    float clampVolume(float v) {
        if (v < 0.0f) return 0.0f;
        if (v > 1.0f) return 1.0f;
        return v;
    }
}

bool BgmPlayer::playLoop(const char* path, float volume) {
    @autoreleasepool {
        stop();

        if (!path || path[0] == '\0') {
            return false;
        }

        NSString* filePath = [NSString stringWithUTF8String:path];
        if (!filePath) {
            return false;
        }

        NSURL* url = [NSURL fileURLWithPath:filePath];
        if (!url) {
            return false;
        }

        NSError* error = nil;
        gPlayer = [[AVAudioPlayer alloc] initWithContentsOfURL:url error:&error];
        if (!gPlayer || error) {
            gPlayer = nil;
            return false;
        }

        gPlayer.numberOfLoops = -1;
        gPlayer.volume = clampVolume(volume);
        [gPlayer prepareToPlay];

        if (![gPlayer play]) {
            gPlayer = nil;
            return false;
        }

        return true;
    }
}

void BgmPlayer::stop() {
    @autoreleasepool {
        if (gPlayer) {
            [gPlayer stop];
            gPlayer = nil;
        }
    }
}

void BgmPlayer::setVolume(float volume) {
    @autoreleasepool {
        if (gPlayer) {
            gPlayer.volume = clampVolume(volume);
        }
    }
}

bool BgmPlayer::isPlaying() {
    @autoreleasepool {
        return gPlayer ? gPlayer.playing : false;
    }
}