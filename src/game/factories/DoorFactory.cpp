#include "DoorFactory.h"

#include "../components/identity/DoorTagComponent.h"
#include "../components/gameplay/DoorComponent.h"
#include "../components/core/TransformComponent.h"

Entity& DoorFactory::createDoor(Entity& entity,
                                AssetManager& assets,
                                float x,
                                float y,
                                float w,
                                float h,
                                const std::string& targetMap,
                                const std::string& targetSpawn) {
    entity.addComponent<DoorTagComponent>();
    entity.addComponent<TransformComponent>(x, y, w, h);
    entity.addComponent<DoorComponent>(targetMap, targetSpawn);

    // entity.addComponent<SpriteComponent>(assets.getTexture(AssetPaths::DOOR));


    return entity;
}