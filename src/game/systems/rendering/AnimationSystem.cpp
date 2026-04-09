#include "AnimationSystem.h"

#include "../../../engine/resources/AssetManager.h"
#include "../../components/core/AnimationComponent.h"
#include "../../components/core/SpriteComponent.h"
#include "../../components/core/PhysicsComponent.h"
#include "../../components/core/HealthComponent.h"
#include "../../components/core/InputComponent.h"
#include "../../components/gameplay/EnemyAIComponent.h"
#include "../../components/identity/PlayerTagComponent.h"
#include "../../components/identity/EnemyTagComponent.h"
#include "../../components/gameplay/WeaponComponent.h"

#include <cmath>
#include <string>

static void advanceAnimation(AnimationComponent& anim,
                             SpriteComponent& sprite,
                             AssetManager& assets,
                             float dt) {
    const AnimationClip* clip = anim.getCurrentClip();
    if (!clip) {
        return;
    }

    SDL_Texture* texture = assets.getTexture(clip->spritesheetPath);
    if (texture && sprite.getTexture() != texture) {
        sprite.setTexture(texture);
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
        static_cast<float>(anim.currentFrame * clip->frameWidth),
        static_cast<float>(clip->row * clip->frameHeight),
        static_cast<float>(clip->frameWidth),
        static_cast<float>(clip->frameHeight)
    };

    sprite.setSourceRect(srcRect);
}

static void playIfChanged(AnimationComponent& anim, const std::string& name, bool restart = false) {
    if (!anim.hasClip(name)) {
        return;
    }

    if (anim.getCurrentClipName() != name) {
        anim.play(name, restart);
    }
}

static void updatePlayerAnimation(Entity& entity) {
    if (!entity.hasComponent<AnimationComponent>() ||
        !entity.hasComponent<HealthComponent>() ||
        !entity.hasComponent<WeaponComponent>()) {
        return;
    }

    auto& anim = entity.getComponent<AnimationComponent>();
    auto& health = entity.getComponent<HealthComponent>();
    auto& weapon = entity.getComponent<WeaponComponent>();

    if (health.isDead()) {
        playIfChanged(anim, "dead", true);
        return;
    }

    if (entity.hasComponent<PhysicsComponent>()) {
        auto& physics = entity.getComponent<PhysicsComponent>();
        if (physics.getMoveX() < 0.0f) {
            anim.flipX = true;
        } else if (physics.getMoveX() > 0.0f) {
            anim.flipX = false;
        }
    }

    if (anim.getCurrentClipName() == "attack" && !anim.finished) {
        return;
    }

    if (anim.getCurrentClipName() == "reload" && !anim.finished) {
        return;
    }

    if (anim.getCurrentClipName() == "dash" && !anim.finished) {
        return;
    }

    if (weapon.isReloading()) {
        playIfChanged(anim, "reload", true);
        return;
    }

    if (weapon.isAttacking()) {
        playIfChanged(anim, "attack", true);
        return;
    }

    if (entity.hasComponent<InputComponent>()) {
        auto& input = entity.getComponent<InputComponent>();
        if (input.isDashing()) {
            playIfChanged(anim, "dash", true);
            return;
        }
    }

    if (entity.hasComponent<PhysicsComponent>()) {
        auto& physics = entity.getComponent<PhysicsComponent>();

        const float enterRunThreshold = 0.20f;
        const float exitRunThreshold = 0.08f;
        const float absMoveX = std::fabs(physics.getMoveX());

        std::string desiredClip = "idle";

        if (!physics.isGrounded()) {
            desiredClip = "jump";
        } else if (absMoveX > enterRunThreshold) {
            desiredClip = "run";
        } else if (anim.getCurrentClipName() == "run" && absMoveX > exitRunThreshold) {
            desiredClip = "run";
        }

        playIfChanged(anim, desiredClip);
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

    if (ai.direction < 0) {
        anim.flipX = true;
    } else if (ai.direction > 0) {
        anim.flipX = false;
    }

    if (health.isDead()) {
        playIfChanged(anim, "dead", true);
        return;
    }

    const std::string current = anim.getCurrentClipName();
    if ((current == "attack" || current == "hurt" || current == "dead") && !anim.finished) {
        return;
    }

    switch (ai.state) {
        case EnemyState::Idle:
            playIfChanged(anim, "idle");
            break;

        case EnemyState::Patrol:
        case EnemyState::Chase:
            playIfChanged(anim, "run");
            break;

        case EnemyState::Attack:
            playIfChanged(anim, "attack", true);
            break;

        case EnemyState::Hurt:
            playIfChanged(anim, "hurt", true);
            break;

        case EnemyState::Dead:
            playIfChanged(anim, "dead", true);
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