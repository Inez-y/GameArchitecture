#include "AnimationSystem.h"

#include "../../../engine/resources/AssetManager.h"
#include "../../components/core/AnimationComponent.h"
#include "../../components/core/SpriteComponent.h"
#include "../../components/core/PhysicsComponent.h"
#include "../../components/core/HealthComponent.h"
#include "../../components/gameplay/EnemyAIComponent.h"
#include "../../components/identity/PlayerTagComponent.h"
#include "../../components/identity/EnemyTagComponent.h"

static void advanceAnimation(AnimationComponent& anim,
                             SpriteComponent& sprite,
                             AssetManager& assets,
                             float dt) {
    const AnimationClip* clip = anim.getCurrentClip();
    if (!clip) {
        return;
    }

    SDL_Texture* texture = assets.getTexture(clip->spritesheetPath);
    if (!texture) {
        return;
    }
    sprite.setTexture(texture);

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
        static_cast<float>(anim.currentFrame * clip->frameWidth),
        static_cast<float>(clip->row * clip->frameHeight),
        static_cast<float>(clip->frameWidth),
        static_cast<float>(clip->frameHeight)
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

        if (!physics.isGrounded()) {
            if (anim.hasClip("jump")) {
                anim.play("jump");
            }
            return;
        }

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
    if (!context.assetManager) {
        return;
    }

    for (auto& e : manager.getEntities()) {
        if (!e->hasComponent<SpriteComponent>() ||
            !e->hasComponent<AnimationComponent>()) {
            continue;
        }

        if (e->hasComponent<PlayerTagComponent>()) {
            updatePlayerAnimation(*e);
        } else if (e->hasComponent<EnemyTagComponent>()) {
            updateEnemyAnimation(*e);
        }

        auto& sprite = e->getComponent<SpriteComponent>();
        auto& anim = e->getComponent<AnimationComponent>();

        advanceAnimation(anim, sprite, *context.assetManager, dt);
    }

    if (context.playerEntity &&
        context.playerEntity->hasComponent<SpriteComponent>() &&
        context.playerEntity->hasComponent<AnimationComponent>() &&
        !context.playerEntity->hasComponent<PlayerTagComponent>()) {
        auto& sprite = context.playerEntity->getComponent<SpriteComponent>();
        auto& anim = context.playerEntity->getComponent<AnimationComponent>();
        advanceAnimation(anim, sprite, *context.assetManager, dt);
    }
}