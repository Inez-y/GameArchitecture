#ifndef LASTCARRIAGE_ANIMATIONCLIPS_H
#define LASTCARRIAGE_ANIMATIONCLIPS_H

#pragma once

#include "../components/core/AnimationComponent.h"
#include "AssetPaths.h"
#include <string>

namespace AnimationPresets {

    struct PlayerAnimationSet {
        AnimationClip idle;
        AnimationClip run;
        AnimationClip attack;
        AnimationClip hurt;
        AnimationClip dead;
        AnimationClip jump;
        AnimationClip reload;
        AnimationClip dash;
    };

    struct EnemyAnimationSet {
        AnimationClip idle;
        AnimationClip run;
        AnimationClip attack;
        AnimationClip hurt;
        AnimationClip dead;
    };

    inline const PlayerAnimationSet Player{
        {AssetPaths::PLAYER_IDLE_SPRITESHEET,   0, 228, 192, 23, 0.15f, true},
        {AssetPaths::PLAYER_RUN_SPRITESHEET,    0, 228, 192, 8,  0.07f, true},
        {AssetPaths::PLAYER_ATTACK_SPRITESHEET, 0, 228, 192, 16, 0.06f, false},
        {AssetPaths::PLAYER_HURT_SPRITESHEET,   0, 228, 192, 4,  0.09f, false},
        {AssetPaths::PLAYER_DEAD_SPRITESHEET,   0, 228, 192, 3,  0.12f, false},
        {AssetPaths::PLAYER_JUMP_SPRITESHEET,   0, 228, 192, 8,  0.09f, false},
        {AssetPaths::PLAYER_RELOAD_SPRITESHEET, 0, 228, 192, 10,  0.12f, false}, // 2280 / 228
        {AssetPaths::PLAYER_DASH_SPRITESHEET,   0, 228, 192, 8,  0.07f, false} // 1824 / 220
    };

    inline const EnemyAnimationSet PatrolEnemy{
        {AssetPaths::ENEMY_IDLE_SPRITESHEET,          0, 228, 192, 12, 0.10f, true},
        {AssetPaths::ENEMY_RUN_SPRITESHEET,           0, 228, 192, 12, 0.08f, true},
        {AssetPaths::ENEMY_ATTACK_PATROL_SPRITESHEET, 0, 228, 192, 8,  0.07f, false},
        {AssetPaths::ENEMY_HURT_SPRITESHEET,          0, 228, 192, 4,  0.10f, false},
        {AssetPaths::ENEMY_DEAD_SPRITESHEET,          0, 228, 192, 8,  0.12f, false}
    };

    inline const EnemyAnimationSet ShooterEnemy{
        {AssetPaths::ENEMY_IDLE_SPRITESHEET,           0, 228, 192, 12, 0.10f, true},
        {AssetPaths::ENEMY_RUN_SPRITESHEET,            0, 228, 192, 12, 0.08f, true},
        {AssetPaths::ENEMY_ATTACK_SHOOTER_SPRITESHEET, 0, 228, 192, 8,  0.07f, false},
        {AssetPaths::ENEMY_HURT_SPRITESHEET,           0, 228, 192, 4,  0.10f, false},
        {AssetPaths::ENEMY_DEAD_SPRITESHEET,           0, 228, 192, 8,  0.12f, false}
    };

    inline const EnemyAnimationSet FlyingEnemy{
        {AssetPaths::ENEMY_IDLE_SPRITESHEET,          0, 228, 192, 12, 0.10f, true},
        {AssetPaths::ENEMY_RUN_SPRITESHEET,           0, 228, 192, 12, 0.08f, true},
        {AssetPaths::ENEMY_ATTACK_FLYING_SPRITESHEET, 0, 228, 192, 8,  0.07f, false},
        {AssetPaths::ENEMY_HURT_SPRITESHEET,          0, 228, 192, 4,  0.10f, false},
        {AssetPaths::ENEMY_DEAD_SPRITESHEET,          0, 228, 192, 8,  0.12f, false}
    };

    inline const EnemyAnimationSet BossPatrol{
        {AssetPaths::BOSS_IDLE_SPRITESHEET,          0, 266, 203, 3990/266, 0.10f, true},
        {AssetPaths::BOSS_RUN_SPRITESHEET,           0, 266, 203, 2128/266, 0.09f, true},
        {AssetPaths::BOSS_ATTACK_PATROL_SPRITESHEET, 0, 266, 203, 5054/266, 0.07f, false},
        {AssetPaths::BOSS_HURT_SPRITESHEET,          0, 266, 203, 532/266,  0.10f, false},
        {AssetPaths::BOSS_DEAD_SPRITESHEET,          0, 266, 203, 1330/266, 0.14f, false}
    };

    inline const EnemyAnimationSet BossShooter{
        {AssetPaths::BOSS_IDLE_SPRITESHEET,           0, 266, 203, 3990/266, 0.12f, true},
        {AssetPaths::BOSS_RUN_SPRITESHEET,            0, 266, 203, 2128/266, 0.09f, true},
        {AssetPaths::BOSS_ATTACK_SHOOTER_SPRITESHEET, 0, 266, 203, 1506/266, 0.07f, false},
        {AssetPaths::BOSS_HURT_SPRITESHEET,           0, 266, 203, 532/266,  0.10f, false},
        {AssetPaths::BOSS_DEAD_SPRITESHEET,           0, 266, 203, 1330/266, 0.14f, false}
    };

    inline const EnemyAnimationSet& enemySetForType(const std::string& typeName) {
        if (typeName == "Flying") return FlyingEnemy;
        if (typeName == "Shooter") return ShooterEnemy;
        if (typeName == "BossShooter") return BossShooter;
        if (typeName == "Boss") return BossPatrol;
        return PatrolEnemy;
    }

    inline void applySet(AnimationComponent& anim, const PlayerAnimationSet& set) {
        anim.addClip("idle", set.idle);
        anim.addClip("run", set.run);
        anim.addClip("attack", set.attack);
        anim.addClip("hurt", set.hurt);
        anim.addClip("dead", set.dead);
        anim.addClip("jump", set.jump);
        anim.addClip("reload", set.reload);
        anim.addClip("dash", set.dash);
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
