#ifndef LASTCARRIAGE_ENEMY_H
#define LASTCARRIAGE_ENEMY_H
#pragma once
#include <SDL3/SDL.h>

enum class EnemyState {
    Idle,
    Patrol,
    Chase,
    Attack,
    Hurt,
    Dead
};

class Enemy {
public:
    Enemy();

    bool init(SDL_Renderer* renderer, const char* texturePath, float startX, float startY, float patrolLeft, float patrolRight);
    void update(float deltaTime, float playerX, float playerY);
    void render(SDL_Renderer* renderer, const SDL_FRect& camera);
    void clean();

    void takeDamage(int damage);

    SDL_FRect getBounds() const;
    EnemyState getState() const;
    bool isDead() const;

private:
    // Enemy states
    void updateIdle(float deltaTime, float playerX, float playerY);
    void updatePatrol(float deltaTime, float playerX, float playerY);
    void updateChase(float deltaTime, float playerX, float playerY);
    void updateAttack(float deltaTime, float playerX, float playerY);
    void updateHurt(float deltaTime, float playerX, float playerY); // stun
    void updateDead(float deltaTime, float playerX, float playerY);

    float distanceToPlayer(float playerX, float playerY) const;
    void changeState(EnemyState newState);

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

    float attackTimer;
    float attackDuration;
    bool damageAppliedThisAttack;

    float hurtTimer;
    float hurtDuration;

    int health;
};

#endif //LASTCARRIAGE_ENEMY_H