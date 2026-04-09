cmake_minimum_required(VERSION 4.1)
project(LastCarriage)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#Tell CMake where Homebrew libraries
set(CMAKE_PREFIX_PATH "/opt/homebrew/opt/sdl3")
set(CMAKE_PREFIX_PATH "/opt/homebrew/opt/sdl3_image")
set(CMAKE_PREFIX_PATH "/opt/homebrew/opt/sdl3_ttf")

include_directories("/opt/homebrew/opt/sdl3/include")
include_directories("/opt/homebrew/opt/sdl3_image/include")
include_directories("/opt/homebrew/opt/sdl3_ttf/include")

link_directories("/opt/homebrew/opt/sdl3/lib")
link_directories("/opt/homebrew/opt/sdl3_image/lib")
link_directories("/opt/homebrew/opt/sdl3_ttf/lib")

#Find SLD3
find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)
find_package(SDL3_ttf REQUIRED)
find_package(SDL2_mixer REQUIRED)

add_executable(LastCarriage src
        src/main.cpp
        src/Game.cpp
        src/Game.h
        src/GameContext.h
        src/vendor/tinyxml2.h
        src/vendor/tinyxml2.cpp
        src/engine/core/Camera.h
        src/engine/ecs/Component.h
        src/engine/ecs/Entity.h
        src/engine/ecs/Manager.h

        src/game/map/runtime/Map.h
        src/game/map/runtime/Map.cpp
        src/game/components/core/TransformComponent.h
        src/game/components/core/HealthComponent.h
        src/game/components/core/SpriteComponent.h
        src/game/components/core/PhysicsComponent.h
        src/game/components/gameplay/WeaponComponent.h
        src/game/components/core/InputComponent.h
        src/game/components/identity/PlayerTagComponent.h
        src/game/components/identity/EnemyTagComponent.h
        src/game/components/gameplay/EnemyAIComponent.h
        src/game/components/gameplay/ItemComponent.h
        src/game/components/identity/ItemTagComponent.h
        src/game/components/gameplay/DoorComponent.h
        src/game/components/identity/DoorTagComponent.h
        src/game/components/gameplay/BulletComponent.h
        src/game/components/identity/BulletTagComponent.h
        src/game/factories/BulletFactory.cpp
        src/game/factories/BulletFactory.h
        src/game/factories/ItemFactory.cpp
        src/game/factories/ItemFactory.h
        src/game/factories/DoorFactory.cpp
        src/game/factories/DoorFactory.h
        src/game/factories/EnemyFactory.cpp
        src/game/factories/EnemyFactory.h
        src/game/factories/PlayerFactory.cpp
        src/game/factories/PlayerFactory.h

        src/game/systems/interaction/ItemSystem.h
        src/game/systems/combat/CombatSystem.h
        src/game/systems/interaction/DoorSystem.h
        src/game/systems/rendering/RenderSystem.h
        src/game/systems/movement/MovementSystem.h
        src/game/systems/interaction/DoorSystem.cpp
        src/game/systems/interaction/ItemSystem.cpp
        src/game/systems/combat/CombatSystem.cpp
        src/game/systems/rendering/RenderSystem.cpp
        src/game/systems/movement/MovementSystem.cpp
        src/game/systems/physics/CollisionSystem.cpp
        src/game/systems/physics/CollisionSystem.h
        src/game/map/runtime/StageManager.h
        src/game/map/runtime/StageManager.cpp
        src/game/map/data/MapData.h
        src/game/map/loading/MapLoader.h
        src/game/map/loading/MapLoader.cpp
        src/game/data/AssetPaths.h
        src/engine/resources/AssetManager.cpp
        src/engine/resources/AssetManager.h
        src/game/scenes/IScene.h
        src/game/scenes/PlayScene.cpp
        src/game/scenes/PlayScene.h
        src/game/systems/movement/EnemyMovementLogic.cpp
        src/game/systems/movement/EnemyMovementLogic.h
        src/game/components/core/AnimationComponent.h
        src/game/systems/rendering/AnimationSystem.cpp
        src/game/systems/rendering/AnimationSystem.h
        src/game/data/AnimationClips.h
        src/game/systems/audio/BgmPlayer.h
        src/game/systems/audio/BgmPlayer.mm
        src/game/systems/audio/AudioSystem.cpp
        src/game/systems/audio/AudioSystem.h
)

target_link_libraries(LastCarriage
        SDL3::SDL3
        SDL3_image::SDL3_image
        SDL3_ttf::SDL3_ttf)

# Audio
find_library(AVFOUNDATION_FRAMEWORK AVFoundation)
find_library(FOUNDATION_FRAMEWORK Foundation)

target_sources(LastCarriage PRIVATE
        src/game/systems/audio/BgmPlayer.mm
)

target_link_libraries(LastCarriage
        ${AVFOUNDATION_FRAMEWORK}
        ${FOUNDATION_FRAMEWORK}
)

