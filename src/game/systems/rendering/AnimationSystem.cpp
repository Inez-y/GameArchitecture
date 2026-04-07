#include "AnimationSystem.h"

#include "../../components/core/AnimationComponent.h"
#include "../../components/core/SpriteComponent.h"
#include "../../components/core/PhysicsComponent.h"
#include "../../components/core/HealthComponent.h"
#include "../../components/core/InputComponent.h"
#include "../../components/gameplay/EnemyAIComponent.h"
#include "../../components/identity/PlayerTagComponent.h"
#include "../../components/identity/EnemyTagComponent.h"

static void advanceAnimation(AnimationComponent& anim, SpriteComponent& sprite, float dt) {
    const AnimationClip* clip = anim.getCurrentClip();
    if (!clip) {
        return;
    }

    anim.timer += dt;

    while (anim.timer >= clip->frameDuration) {
        anim.timer -= clip->frameDuration;
        anim.currentFrame++;

        if (anim.currentFrame >= clip->frameCount) {
            if (clip->loop) {
                anim.currentFrame = 0;
            } else {
                anim.currentFrame = clip->frameCount - 1;
                anim.finished = true;
            }
        }
    }

    SDL_FRect srcRect{
        static_cast<float>(anim.currentFrame * anim.frameWidth),
        static_cast<float>(clip->row * anim.frameHeight),
        static_cast<float>(anim.frameWidth),
        static_cast<float>(anim.frameHeight)
    };

    sprite.setSourceRect(srcRect);
}

static void updatePlayerAnimation(Entity& entity) {
    if (!entity.hasComponent<AnimationComponent>() ||
        !entity.hasComponent<HealthComponent>()) {
        return;
    }

    auto& anim = entity.getComponent<AnimationComponent>();
    auto& health = entity.getComponent<HealthComponent>();

    if (health.isDead()) {
        if (anim.hasClip("dead")) {
            anim.play("dead");
        }
        return;
    }

    if (entity.hasComponent<PhysicsComponent>()) {
        auto& physics = entity.getComponent<PhysicsComponent>();
        if (physics.getMoveX() != 0.0f) {
            if (anim.hasClip("run")) {
                anim.play("run");
            }
        } else {
            if (anim.hasClip("idle")) {
                anim.play("idle");
            }
        }
    } else {
        if (anim.hasClip("idle")) {
            anim.play("idle");
        }
    }
}

static void updateEnemyAnimation(Entity& entity) {
    if (!entity.hasComponent<AnimationComponent>() ||
        !entity.hasComponent<EnemyAIComponent>() ||
        !entity.hasComponent<HealthComponent>()) {
        return;
    }

    auto& anim = entity.getComponent<AnimationComponent>();
    auto& ai = entity.getComponent<EnemyAIComponent>();
    auto& health = entity.getComponent<HealthComponent>();

    if (health.isDead()) {
        if (anim.hasClip("dead")) {
            anim.play("dead");
        }
        return;
    }

    switch (ai.state) {
        case EnemyState::Idle:
            if (anim.hasClip("idle")) {
                anim.play("idle");
            }
            break;

        case EnemyState::Patrol:
        case EnemyState::Chase:
            if (anim.hasClip("run")) {
                anim.play("run");
            }
            break;

        case EnemyState::Attack:
            if (anim.hasClip("attack")) {
                anim.play("attack");
            }
            break;

        case EnemyState::Hurt:
            if (anim.hasClip("hurt")) {
                anim.play("hurt");
            }
            break;

        case EnemyState::Dead:
            if (anim.hasClip("dead")) {
                anim.play("dead");
            }
            break;
    }
}

void AnimationSystem::update(Manager& manager, GameContext& context, float dt) {
    for (auto& e : manager.getEntities()) {
        if (!e->hasComponent<SpriteComponent>() ||
            !e->hasComponent<AnimationComponent>()) {
            continue;
        }

        auto& sprite = e->getComponent<SpriteComponent>();
        auto& anim = e->getComponent<AnimationComponent>();

        if (e->hasComponent<PlayerTagComponent>()) {
            updatePlayerAnimation(*e);
        } else if (e->hasComponent<EnemyTagComponent>()) {
            updateEnemyAnimation(*e);
        }

        advanceAnimation(anim, sprite, dt);
    }

    if (context.playerEntity &&
        context.playerEntity->hasComponent<SpriteComponent>() &&
        context.playerEntity->hasComponent<AnimationComponent>()) {
        auto& sprite = context.playerEntity->getComponent<SpriteComponent>();
        auto& anim = context.playerEntity->getComponent<AnimationComponent>();

        if (context.playerEntity->hasComponent<PlayerTagComponent>()) {
            updatePlayerAnimation(*context.playerEntity);
        }

        advanceAnimation(anim, sprite, dt);
    }
}