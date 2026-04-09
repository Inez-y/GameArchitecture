#ifndef LASTCARRIAGE_ENEMYAICOMPONENT_H
#define LASTCARRIAGE_ENEMYAICOMPONENT_H
#pragma once

#include "../../../engine/ecs/Component.h"
#include <cmath>
#include <string>

enum class EnemyType {
    Patrol,
    Shooter,
    Flying,
    Boss
};

enum class EnemyState {
    Idle,
    Patrol,
    Chase,
    Attack,
    Hurt,
    Dead
};

enum class BossAttackType {
    None,
    BurstShot,
    DashAttack
};

class EnemyAIComponent : public Component {
public:
    EnemyType type;
    EnemyState state;

    float patrolLeft;
    float patrolRight;

    int direction;

    float idleTimer;
    float idleDuration;

    float chaseRange;
    float stopChaseRange;
    float attackRange;

    float attackTimer;
    float attackDuration;
    bool damageAppliedThisAttack;
    bool attackHitThisFrame;
    bool shotThisFrame;

    float hurtTimer;
    float hurtDuration;

    BossAttackType currentBossAttack;
    float bossAttackTimer;
    float bossAttackCooldown;
    int bossAttackPhase;

    EnemyAIComponent(EnemyType enemyType = EnemyType::Patrol,
                     float leftBound = 0.0f,
                     float rightBound = 0.0f)
        : type(enemyType),
          state(EnemyState::Patrol),
          patrolLeft(leftBound),
          patrolRight(rightBound),
          direction(1),
          idleTimer(0.0f),
          idleDuration(1.0f),
          chaseRange(220.0f),
          stopChaseRange(260.0f),
          attackRange(40.0f),
          attackTimer(0.0f),
          attackDuration(0.8f),
          damageAppliedThisAttack(false),
          attackHitThisFrame(false),
          shotThisFrame(false),
          hurtTimer(0.0f),
          hurtDuration(0.5f),
          currentBossAttack(BossAttackType::None),
          bossAttackTimer(0.0f),
          bossAttackCooldown(1.5f),
          bossAttackPhase(0) {
    }

    void init() override {
        configureByType();
    }

    void configureByType() {
        state = EnemyState::Patrol;
        attackRange = 40.0f;
        chaseRange = 220.0f;
        stopChaseRange = 260.0f;
        attackDuration = 0.8f;
        hurtDuration = 0.45f;

        if (type == EnemyType::Boss) {
            state = EnemyState::Idle;
            attackRange = 60.0f;
            chaseRange = 260.0f;
            stopChaseRange = 300.0f;
            attackDuration = 1.0f;
            hurtDuration = 0.5f;
        }
        else if (type == EnemyType::Shooter) {
            state = EnemyState::Idle;
            attackRange = 250.0f;
            chaseRange = 300.0f;
            stopChaseRange = 340.0f;
            attackDuration = 1.0f;
            hurtDuration = 0.45f;
        }
        else if (type == EnemyType::Flying) {
            state = EnemyState::Chase;
            attackRange = 60.0f;
            chaseRange = 260.0f;
            stopChaseRange = 320.0f;
            attackDuration = 0.75f;
            hurtDuration = 0.4f;
        }
        else {
            state = EnemyState::Patrol;
            attackRange = 40.0f;
            chaseRange = 220.0f;
            stopChaseRange = 260.0f;
            attackDuration = 0.8f;
            hurtDuration = 0.45f;
        }
    }

    void beginFrame() {
        attackHitThisFrame = false;
        shotThisFrame = false;
    }

    void changeState(EnemyState newState) {
        if (state == newState) {
            return;
        }

        state = newState;

        if (state == EnemyState::Idle) {
            idleTimer = 0.0f;
        }
        else if (state == EnemyState::Attack) {
            attackTimer = 0.0f;
            damageAppliedThisAttack = false;
        }
        else if (state == EnemyState::Hurt) {
            hurtTimer = 0.0f;
        }
    }

    bool isDead() const {
        return state == EnemyState::Dead;
    }

    void resetAttackHit() {
        attackHitThisFrame = false;
    }

    void resetShotThisFrame() {
        shotThisFrame = false;
    }

    float distanceTo(float selfX, float selfY, float otherX, float otherY) const {
        float dx = otherX - selfX;
        float dy = otherY - selfY;
        return std::sqrt(dx * dx + dy * dy);
    }
};
#endif //LASTCARRIAGE_ENEMYAICOMPONENT_H