#ifndef LASTCARRIAGE_ANIMATIONCLIPS_H
#define LASTCARRIAGE_ANIMATIONCLIPS_H

#pragma once

#include "../components/core/AnimationComponent.h"

namespace AnimationPresets {

    struct AnimationSet {
        int frameWidth;
        int frameHeight;
        AnimationClip idle;
        AnimationClip run;
        AnimationClip attack;
        AnimationClip hurt;
        AnimationClip dead;
    };

    inline const AnimationSet Player{
        32, 32,
        {0, 4, 0.15f, true},   // idle
        {1, 6, 0.10f, true},   // run
        {2, 4, 0.08f, false},  // attack
        {3, 2, 0.12f, false},  // hurt
        {4, 4, 0.18f, false}   // dead
    };

    inline const AnimationSet Enemy{
        32, 32,
        {0, 4, 0.18f, true},   // idle
        {1, 6, 0.12f, true},   // run
        {2, 4, 0.10f, false},  // attack
        {3, 2, 0.12f, false},  // hurt
        {4, 4, 0.18f, false}   // dead
    };

    inline const AnimationSet Boss{
        64, 64,
        {0, 4, 0.18f, true},   // idle
        {1, 6, 0.10f, true},   // run
        {2, 6, 0.09f, false},  // attack
        {3, 2, 0.12f, false},  // hurt
        {4, 6, 0.16f, false}   // dead
    };

    inline void applySet(AnimationComponent& anim, const AnimationSet& set) {
        anim.addClip("idle", set.idle);
        anim.addClip("run", set.run);
        anim.addClip("attack", set.attack);
        anim.addClip("hurt", set.hurt);
        anim.addClip("dead", set.dead);
        anim.play("idle");
    }
}

#endif // LASTCARRIAGE_ANIMATIONCLIPS_H