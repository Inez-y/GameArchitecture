#include "DoorSystem.h"

#include "../components/DoorTagComponent.h"
#include "../components/DoorComponent.h"
#include "../components/TransformComponent.h"
#include "../components/PlayerTagComponent.h"

bool DoorSystem::overlaps(const SDL_FRect& a, const SDL_FRect& b) {
    return a.x < b.x + b.w &&
           a.x + a.w > b.x &&
           a.y < b.y + b.h &&
           a.y + a.h > b.y;
}

void DoorSystem::update(Manager& manager, GameContext& context, float dt) {
    if (!context.playerEntity || !context.map) {
        return;
    }

    if (context.doorTimer > 0.0f) {
        context.doorTimer -= dt;
        if (context.doorTimer < 0.0f) {
            context.doorTimer = 0.0f;
        }
        return;
    }

    auto& playerTransform = context.playerEntity->getComponent<TransformComponent>();
    const SDL_FRect playerBounds = playerTransform.getRect();

    for (auto& e : manager.getEntities()) {
        if (!e->hasComponent<DoorTagComponent>()) {
            continue;
        }

        auto& transform = e->getComponent<TransformComponent>();
        auto& door = e->getComponent<DoorComponent>();

        if (!door.isActive() || door.getTargetMap().empty()) {
            continue;
        }

        if (overlaps(playerBounds, transform.getRect())) {
            context.stageChangeRequested = true;
            context.requestedStagePath = door.getTargetMap();
            context.doorTimer = context.doorCooldown;
            break;
        }
    }
}