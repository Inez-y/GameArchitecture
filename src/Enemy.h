#ifndef LASTCARRIAGE_ENEMY_H
#define LASTCARRIAGE_ENEMY_H
#pragma once
#include <string>
#include <SDL3/SDL.h>

#include "Map.h"

enum class EnemyState {
    Idle,
    Patrol,
    Chase,
    Attack,
    Hurt,
    Dead
};

enum class EnemyType {
    Patrol,
    Shooter,
    Flying,
    Boss
};

enum class BossAttackType {
    None,
    BurstShot,
    DashAttack,
    RainAttack
};

// Called in Game.cpp
EnemyType stringToEnemyType(const std::string& type);
const char* enemyTexturePath(EnemyType type);


class Enemy {
public:
    Enemy();

    bool init(SDL_Renderer* renderer, const char* texturePath,
              float startX, float startY,
              float patrolLeft, float patrolRight,
              EnemyType enemyType);
    void update(float deltaTime, float playerX, float playerY, const Map& map);
    void render(SDL_Renderer* renderer, const SDL_FRect& camera);
    void clean();

    void takeDamage(int damage);

    SDL_FRect getBounds() const;
    EnemyState getState() const;

    bool isDead() const;
    bool didAttackHit() const;
    void resetAttackHit();

    void applyGravity(float deltaTime, const Map& map);

    bool didShootThisFrame() const;
    void resetShotThisFrame();


private:
    // Enemy states
    void updateIdle(float deltaTime, float playerX, float playerY);
    void updatePatrol(float deltaTime, float playerX, float playerY);
    void updateChase(float deltaTime, float playerX, float playerY);
    void updateAttack(float deltaTime, float playerX, float playerY);
    void updateHurt(float deltaTime, float playerX, float playerY); // stun
    void updateDead(float deltaTime, float playerX, float playerY);

    // Enemy Types
    void updatePatrolType(float deltaTime, float playerX, float playerY);
    void updateShooterType(float deltaTime, float playerX, float playerY);
    void updateFlyingType(float deltaTime, float playerX, float playerY);
    void updateBossType(float deltaTime, float playerX, float playerY);

    // Attack functions per type
    void updatePatrolAttack(float deltaTime, float playerX, float playerY);
    void updateShooterAttack(float deltaTime, float playerX, float playerY);
    void updateFlyingAttack(float deltaTime, float playerX, float playerY);
    void updateBossAttack(float deltaTime, float playerX, float playerY);
    void updateShooterIdle(float deltaTime, float playerX, float playerY);

    // Boss
    void updateBoss(float deltaTime, float playerX, float playerY);
    void updateBossIdle(float deltaTime, float playerX, float playerY);
    void updateBossBurstShot(float deltaTime, float playerX, float playerY);
    void updateBossDashAttack(float deltaTime, float playerX, float playerY);

    float distanceToPlayer(float playerX, float playerY) const;
    void changeState(EnemyState newState);

    bool isGrounded() const;

private:
    SDL_Texture* texture;
    SDL_FRect dstRect;

    float x;
    float y;
    float speed;

    float patrolLeft;
    float patrolRight;
    int direction; // -1 (left) 1 (right)

    // Enemy state logic
    EnemyState state;

    float idleTimer;
    float idleDuration;

    float chaseRange;
    float stopChaseRange;
    float attackRange;
    float detectionRange;

    float attackTimer;
    float attackDuration;
    bool damageAppliedThisAttack;

    float hurtTimer;
    float hurtDuration;

    int health;

    // Boss
    BossAttackType currentBossAttack;
    float bossAttackTimer;
    float bossAttackCooldown;
    int bossAttackPhase;

    EnemyType type;
    bool attackHitThisFrame;

    Map map;
    bool grounded;
    float velocityY;
    float gravity;

    bool shotThisFrame;


};

#endif //LASTCARRIAGE_ENEMY_H