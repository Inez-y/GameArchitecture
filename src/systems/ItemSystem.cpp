#include "ItemSystem.h"

#include <iostream>

#include "../components/ItemTagComponent.h"
#include "../components/ItemComponent.h"
#include "../components/TransformComponent.h"
#include "../components/HealthComponent.h"

bool ItemSystem::overlaps(const SDL_FRect& a, const SDL_FRect& b) {
    return a.x < b.x + b.w &&
           a.x + a.w > b.x &&
           a.y < b.y + b.h &&
           a.y + a.h > b.y;
}

void ItemSystem::applyItemEffect(GameContext& context, Entity& player, ItemType type) {
    auto& playerHealth = player.getComponent<HealthComponent>();

    switch (type) {
        case ItemType::Coin:
            context.coinCount++;
            std::cout << "Picked up coin. Coins: " << context.coinCount << std::endl;
            break;

        case ItemType::Health:
            playerHealth.heal(1);
            std::cout << "Picked up health. HP: " << playerHealth.getHP() << std::endl;
            break;

        case ItemType::Key:
            std::cout << "Picked up key." << std::endl;
            break;
    }
}

void ItemSystem::update(Manager& manager, GameContext& context) {
    if (!context.playerEntity) {
        return;
    }

    auto& playerTransform = context.playerEntity->getComponent<TransformComponent>();
    const SDL_FRect playerBounds = playerTransform.getRect();

    for (auto& e : manager.getEntities()) {
        if (!e->hasComponent<ItemTagComponent>()) {
            continue;
        }

        auto& transform = e->getComponent<TransformComponent>();
        auto& item = e->getComponent<ItemComponent>();

        if (!item.isActive()) {
            continue;
        }

        if (overlaps(playerBounds, transform.getRect())) {
            applyItemEffect(context, *context.playerEntity, item.getType());
            item.pickUp();
            e->destroy();
        }
    }
}