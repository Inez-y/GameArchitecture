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
##
#include_directories("/src") # new
#include_directories("/src/ecs") # new
#include_directories("/src/ecs/system") # new
#include_directories("/src/vendor") # new
#
link_directories("/opt/homebrew/opt/sdl3/lib")
link_directories("/opt/homebrew/opt/sdl3_image/lib")
link_directories("/opt/homebrew/opt/sdl3_ttf/lib")

#Find SLD3
find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)
find_package(SDL3_ttf REQUIRED)

add_executable(LastCarriage src
        src/main.cpp
        src/map/Map.h
        src/map/Map.cpp
        src/Game.cpp
        src/Game.h
        src/vendor/tinyxml2.h
        src/vendor/tinyxml2.cpp
        src/Camera.h
        src/ecs/Component.h
        src/ecs/Entity.h
        src/ecs/Manager.h
        src/components/TransformComponent.h
        src/components/HealthComponent.h
        src/components/SpriteComponent.h
        src/components/PhysicsComponent.h
        src/components/WeaponComponent.h
        src/components/InputComponent.h
        src/factories/PlayerFactory.cpp
        src/factories/PlayerFactory.h
        src/components/PlayerTagComponent.h
        src/components/EnemyTagComponent.h
        src/components/EnemyAIComponent.h
        src/factories/EnemyFactory.cpp
        src/factories/EnemyFactory.h
        src/components/ItemComponent.h
        src/components/ItemTagComponent.h
        src/factories/ItemFactory.cpp
        src/factories/ItemFactory.h
        src/components/BulletComponent.h
        src/components/BulletTagComponent.h
        src/factories/BulletFactory.cpp
        src/factories/BulletFactory.h
        src/components/DoorComponent.h
        src/components/DoorTagComponent.h
        src/factories/DoorFactory.cpp
        src/factories/DoorFactory.h
        src/GameContext.h
        src/systems/ItemSystem.h
        src/systems/CombatSystem.h
        src/systems/DoorSystem.h
        src/systems/RenderSystem.h
        src/systems/MovementSystem.h
        src/systems/DoorSystem.cpp
        src/systems/ItemSystem.cpp
        src/systems/CombatSystem.cpp
        src/systems/RenderSystem.cpp
        src/systems/MovementSystem.cpp
        src/map/StageManager.h
        src/map/StageManager.cpp
)

target_link_libraries(LastCarriage
        SDL3::SDL3
        SDL3_image::SDL3_image
        SDL3_ttf::SDL3_ttf)

