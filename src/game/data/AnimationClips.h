#ifndef LASTCARRIAGE_ANIMATIONCLIPS_H
#define LASTCARRIAGE_ANIMATIONCLIPS_H

#pragma once

#include "../components/core/AnimationComponent.h"
#include "AssetPaths.h"

namespace AnimationPresets {

    struct PlayerAnimationSet {
        AnimationClip idle;
        AnimationClip run;
        AnimationClip attack;
        AnimationClip hurt;
        AnimationClip dead;
        AnimationClip jump;
    };

    struct EnemyAnimationSet {
        AnimationClip idle;
        AnimationClip run;
        AnimationClip attack;
        AnimationClip hurt;
        AnimationClip dead;
    };

    inline const PlayerAnimationSet Player{
        {AssetPaths::PLAYER_IDLE_SPRITESHEET,   0, 228, 192, 23, 0.10f, true}, // 5244 / 228 = 23 frames
        {AssetPaths::PLAYER_RUN_SPRITESHEET,    0, 228, 192, 8, 0.07f, true}, // 1824 / 228 = 8
        {AssetPaths::PLAYER_ATTACK_SPRITESHEET, 0, 228, 192, 16, 0.06f, false}, // 3648 / 228 = 16
        {AssetPaths::PLAYER_HURT_SPRITESHEET,   0, 228, 192, 4,  0.09f, false},// 912 / 228 = 4
        {AssetPaths::PLAYER_DEAD_SPRITESHEET,   0, 228, 192, 3, 0.12f, false}, // 684 / 228 = 3
        {AssetPaths::PLAYER_JUMP_SPRITESHEET,   0, 228, 192, 8,  0.09f, false} // 1824 / 228 = 8
    };

   inline const EnemyAnimationSet EnemyPatrol{
        {AssetPaths::ENEMY_IDLE_SPRITESHEET,           0, 235, 218, 6, 0.10f, true}, // 1410 / 235
        {AssetPaths::ENEMY_RUN_SPRITESHEET,            0, 235, 218, 6, 0.08f, true}, // 1410 / 235
        {AssetPaths::ENEMY_ATTACK_PATROL_SPRITESHEET,  0, 235, 218,  6, 0.07f, false},
        {AssetPaths::ENEMY_HURT_SPRITESHEET,           0, 235, 218,  2, 0.07f, false}, // 470 / 235
        {AssetPaths::ENEMY_DEAD_SPRITESHEET,           0, 235, 218,  5, 0.12f, false} // 1175
    };

    inline const EnemyAnimationSet EnemyShooter{
      {AssetPaths::ENEMY_IDLE_SPRITESHEET,           0, 235, 218, 6, 0.10f, true}, // 1410 / 235
            {AssetPaths::ENEMY_RUN_SPRITESHEET,            0, 235, 218, 6, 0.08f, true}, // 1410 / 235
            {AssetPaths::ENEMY_ATTACK_SHOOTER_SPRITESHEET,  0, 235, 218,  6, 0.07f, false},
            {AssetPaths::ENEMY_HURT_SPRITESHEET,           0, 235, 218,  2, 0.07f, false}, // 470 / 235
            {AssetPaths::ENEMY_DEAD_SPRITESHEET,           0, 235, 218,  5, 0.12f, false} // 1175
    };

    inline const EnemyAnimationSet EnemyFlying{
             {AssetPaths::ENEMY_IDLE_SPRITESHEET,           0, 235, 218, 6, 0.10f, true}, // 1410 / 235
            {AssetPaths::ENEMY_RUN_SPRITESHEET,            0, 235, 218, 6, 0.08f, true}, // 1410 / 235
            {AssetPaths::ENEMY_ATTACK_FLYING_SPRITESHEET,  0, 235, 218,  9, 0.07f, false}, // 2115 / 235
            {AssetPaths::ENEMY_HURT_SPRITESHEET,           0, 235, 218,  2, 0.07f, false}, // 470 / 235
            {AssetPaths::ENEMY_DEAD_SPRITESHEET,           0, 235, 218,  5, 0.12f, false} // 1175
    };

    inline const EnemyAnimationSet BossPatrol{
        {AssetPaths::BOSS_IDLE_SPRITESHEET,            0, 266, 203, 15, 0.12f, true}, // 3990 / 266
        {AssetPaths::BOSS_RUN_SPRITESHEET,             0, 266, 203, 8, 0.09f, true}, // 2128 / 266
        {AssetPaths::BOSS_ATTACK_PATROL_SPRITESHEET,   0, 266, 203, 19, 0.10f, false}, // 5054 / 266
        {AssetPaths::BOSS_HURT_SPRITESHEET,            0, 266, 203,  2, 0.05f, false}, // 532 / 266
        {AssetPaths::BOSS_DEAD_SPRITESHEET,            0, 266, 203, 5, 0.14f, false} // 1330 / 266
    };

    inline const EnemyAnimationSet BossShooter{
            {AssetPaths::BOSS_IDLE_SPRITESHEET,            0, 266, 203, 15, 0.12f, true}, // 3990 / 266
            {AssetPaths::BOSS_RUN_SPRITESHEET,             0, 266, 203, 8, 0.09f, true}, // 2128 / 266
            {AssetPaths::BOSS_ATTACK_SHOOTER_SPRITESHEET,   0, 266, 203, 6, 0.10f, false}, // 1596 / 266
            {AssetPaths::BOSS_HURT_SPRITESHEET,            0, 266, 203,  2, 0.05f, false}, // 532 / 266
            {AssetPaths::BOSS_DEAD_SPRITESHEET,            0, 266, 203, 5, 0.14f, false} // 1330 / 266
    };

    inline void applySet(AnimationComponent& anim, const PlayerAnimationSet& set) {
        anim.addClip("idle", set.idle);
        anim.addClip("run", set.run);
        anim.addClip("attack", set.attack);
        anim.addClip("hurt", set.hurt);
        anim.addClip("dead", set.dead);
        anim.addClip("jump", set.jump);
        anim.play("idle");
    }

    inline void applySet(AnimationComponent& anim, const EnemyAnimationSet& set) {
        anim.addClip("idle", set.idle);
        anim.addClip("run", set.run);
        anim.addClip("attack", set.attack);
        anim.addClip("hurt", set.hurt);
        anim.addClip("dead", set.dead);
        anim.play("idle");
    }
}

#endif // LASTCARRIAGE_ANIMATIONCLIPS_H