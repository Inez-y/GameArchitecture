#include "ItemFactory.h"

#include "../components/identity/ItemTagComponent.h"
#include "../components/gameplay/ItemComponent.h"
#include "../components/core/TransformComponent.h"
#include "../components/core/SpriteComponent.h"

static ItemType stringToItemType(const std::string& type) {
    if (type == "Coin") return ItemType::Coin;
    if (type == "Health") return ItemType::Health;
    if (type == "Key") return ItemType::Key;
    return ItemType::Coin;
}

static const char* itemTexturePath(ItemType type) {
    switch (type) {
        case ItemType::Coin:
            return "../assets/item_coin.png";
        case ItemType::Health:
            return "../assets/item_health.png";
        case ItemType::Key:
            return "../assets/item_key.png";
        default:
            return "../assets/item_coin.png";
    }
}

Entity& ItemFactory::createItem(Entity& entity,
                                SDL_Renderer* renderer,
                                const std::string& typeName,
                                float x,
                                float y) {
    ItemType type = stringToItemType(typeName);

    entity.addComponent<ItemTagComponent>();
    entity.addComponent<TransformComponent>(x, y, 32.0f, 32.0f);
    entity.addComponent<SpriteComponent>(renderer, itemTexturePath(type));
    entity.addComponent<ItemComponent>(type);

    return entity;
}