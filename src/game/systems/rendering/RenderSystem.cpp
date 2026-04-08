#include "RenderSystem.h"

#include "../../components/identity/DoorTagComponent.h"
#include "../../components/gameplay/DoorComponent.h"
#include "../../components/identity/ItemTagComponent.h"
#include "../../components/gameplay/ItemComponent.h"
#include "../../components/identity/EnemyTagComponent.h"
#include "../../components/identity/BulletTagComponent.h"
#include "../../components/gameplay/BulletComponent.h"
#include "../../components/core/HealthComponent.h"
#include "../../components/core/TransformComponent.h"
#include "../../components/core/SpriteComponent.h"
#include "../../components/identity/PlayerTagComponent.h"
#include "../../components/gameplay/WeaponComponent.h"
#include "../../components/core/AnimationComponent.h"

void RenderSystem::render(Manager& manager, const GameContext& context) const {
    if (!context.renderer || !context.camera) {
        return;
    }

    const SDL_FRect cameraRect{
        context.camera->x,
        context.camera->y,
        context.camera->w,
        context.camera->h
    };

    renderDoors(manager, context);
    renderItems(manager, context);
    renderEnemies(manager, context);
    renderBullets(manager, context);
    renderHUD(context);

    if (context.playerEntity &&
        context.playerEntity->hasComponent<SpriteComponent>()) {
        bool flipX = false;
        if (context.playerEntity->hasComponent<AnimationComponent>()) {
            flipX = context.playerEntity->getComponent<AnimationComponent>().flipX;
        }
        context.playerEntity->getComponent<SpriteComponent>().draw(context.renderer, cameraRect, flipX);
    }
}

void RenderSystem::renderDoors(Manager& manager, const GameContext& context) const {
    for (auto& e : manager.getEntities()) {
        if (!e->hasComponent<DoorTagComponent>()) {
            continue;
        }

        auto& door = e->getComponent<DoorComponent>();
        auto rect = e->getComponent<TransformComponent>().getRect();

        if (!door.isActive()) {
            continue;
        }

        rect.x -= context.camera->x;
        rect.y -= context.camera->y;

        SDL_SetRenderDrawColor(context.renderer, 0, 0, 255, 255);
        SDL_RenderFillRect(context.renderer, &rect);
    }
}

void RenderSystem::renderItems(Manager& manager, const GameContext& context) const {
    const SDL_FRect cameraRect{context.camera->x, context.camera->y, context.camera->w, context.camera->h};

    for (auto& e : manager.getEntities()) {
        if (!e->hasComponent<ItemTagComponent>() ||
            !e->hasComponent<ItemComponent>() ||
            !e->hasComponent<SpriteComponent>()) {
            continue;
        }

        auto& item = e->getComponent<ItemComponent>();
        if (item.isActive()) {
            bool flipX = false;
            if (e->hasComponent<AnimationComponent>()) {
                flipX = e->getComponent<AnimationComponent>().flipX;
            }
            e->getComponent<SpriteComponent>().draw(context.renderer, cameraRect, flipX);
        }
    }
}

void RenderSystem::renderEnemies(Manager& manager, const GameContext& context) const {
    const SDL_FRect cameraRect{context.camera->x, context.camera->y, context.camera->w, context.camera->h};

    for (auto& e : manager.getEntities()) {
        if (!e->hasComponent<EnemyTagComponent>() ||
            !e->hasComponent<SpriteComponent>()) {
            continue;
        }

        bool flipX = false;
        if (e->hasComponent<AnimationComponent>()) {
            flipX = e->getComponent<AnimationComponent>().flipX;
        }

        e->getComponent<SpriteComponent>().draw(context.renderer, cameraRect, flipX);
    }
}

void RenderSystem::renderBullets(Manager& manager, const GameContext& context) const {
    for (auto& e : manager.getEntities()) {
        if (!e->hasComponent<BulletTagComponent>() ||
            !e->hasComponent<TransformComponent>() ||
            !e->hasComponent<BulletComponent>()) {
            continue;
        }

        auto& transform = e->getComponent<TransformComponent>();
        auto& bullet = e->getComponent<BulletComponent>();

        if (!bullet.isActive()) {
            continue;
        }

        SDL_FRect rect = transform.getRect();
        rect.x -= context.camera->x;
        rect.y -= context.camera->y;

        if (bullet.getOwner() == BulletOwner::Player) {
            SDL_SetRenderDrawColor(context.renderer, 255, 255, 0, 255);
        } else {
            SDL_SetRenderDrawColor(context.renderer, 255, 100, 100, 255);
        }

        SDL_RenderFillRect(context.renderer, &rect);
    }
}

void RenderSystem::renderHUD(const GameContext& context) const {
    if (!context.renderer || !context.uiFont || !context.playerEntity) {
        return;
    }

    if (!context.playerEntity->hasComponent<HealthComponent>() ||
        !context.playerEntity->hasComponent<WeaponComponent>()) {
        return;
    }

    auto& health = context.playerEntity->getComponent<HealthComponent>();
    auto& weapon = context.playerEntity->getComponent<WeaponComponent>();

    std::string hpString = "HP: " + std::to_string(health.getHP());
    std::string ammoString = "Ammo: " + std::to_string(weapon.getAmmo()) +
                             "/" + std::to_string(weapon.getMaxAmmo());

    if (weapon.isReloading()) {
        ammoString += " (Reloading)";
    }

    SDL_Color color = {255, 255, 255, 255};

    SDL_Surface* hpSurface = TTF_RenderText_Blended(context.uiFont, hpString.c_str(), 0, color);
    SDL_Surface* ammoSurface = TTF_RenderText_Blended(context.uiFont, ammoString.c_str(), 0, color);

    if (!hpSurface || !ammoSurface) {
        if (hpSurface) SDL_DestroySurface(hpSurface);
        if (ammoSurface) SDL_DestroySurface(ammoSurface);
        return;
    }

    SDL_Texture* hpTexture = SDL_CreateTextureFromSurface(context.renderer, hpSurface);
    SDL_Texture* ammoTexture = SDL_CreateTextureFromSurface(context.renderer, ammoSurface);

    if (hpTexture) {
        SDL_FRect hpRect{20.0f, 20.0f, static_cast<float>(hpSurface->w), static_cast<float>(hpSurface->h)};
        SDL_RenderTexture(context.renderer, hpTexture, nullptr, &hpRect);
        SDL_DestroyTexture(hpTexture);
    }

    if (ammoTexture) {
        SDL_FRect ammoRect{20.0f, 55.0f, static_cast<float>(ammoSurface->w), static_cast<float>(ammoSurface->h)};
        SDL_RenderTexture(context.renderer, ammoTexture, nullptr, &ammoRect);
        SDL_DestroyTexture(ammoTexture);
    }

    SDL_DestroySurface(hpSurface);
    SDL_DestroySurface(ammoSurface);
}
