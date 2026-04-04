#include "DoorFactory.h"

#include "../components/DoorTagComponent.h"
#include "../components/DoorComponent.h"
#include "../components/TransformComponent.h"

Entity& DoorFactory::createDoor(Entity& entity,
                                SDL_Renderer* renderer,
                                float x,
                                float y,
                                float w,
                                float h,
                                const std::string& targetMap,
                                const std::string& targetSpawn) {
    entity.addComponent<DoorTagComponent>();
    entity.addComponent<TransformComponent>(x, y, w, h);
    entity.addComponent<DoorComponent>(targetMap, targetSpawn);

    return entity;
}