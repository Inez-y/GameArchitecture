
# LastCarriage

**LastCarriage** is a 2D action game built in C++ with SDL on top of a custom ECS-based game engine.  
The project was developed for an architecture-focused game programming course, with emphasis on clean engine design, modular systems, data-driven level loading, and maintainable code organization.

---

## Overview

The goal of this project is not only to build a playable game, but also to demonstrate a coherent engine architecture that supports gameplay through reusable components, specialized systems, and map-driven content.

The codebase separates:
- core engine utilities
- ECS infrastructure
- gameplay components
- gameplay systems
- entity factories
- map parsing and stage runtime logic

This makes the project easier to extend, test, debug, and present.

---

## Key Features

### Engine Features
- Custom Entity Component System (ECS)
- Modular game systems with clear responsibilities
- Factory-based entity creation
- Data-driven map loading from TMX
- Stage loading and transitions
- Shared runtime context for cross-system coordination
- Organized separation between engine-level and game-level code

### Gameplay Features
- Player movement and combat
- Weapon, ammo, and reload flow
- Various enemies 
- Item pickups
- Door-based level transitions
- Tilemap-based world rendering
- UI rendering for gameplay information

---

## Architecture Summary

The project follows an ECS structure:

- **Entities** - lightweight objects
- **Components** - store data only
- **Systems** - implement behavior
- **Factories** - build common entity archetypes
- **Map loading code** - parses TMX files into usable game data
- **Stage management** - turns map data into active gameplay entities

This architecture is visible in the project structure and class responsibilities. 

The map pipeline is split so that `MapLoader` parses TMX data, `MapData` stores it, `Map` exposes runtime access and rendering, and `StageManager` converts map content into ECS entities such as doors, enemies, items, and player spawn positions. 

---

## Project Structure

```text
src/
├── engine/
│   ├── core/         # camera, shared services
│   ├── ecs/          # entity, component, manager
│   └── resources/    # asset/resource management
│
├── game/
│   ├── components/   # gameplay and identity components
│   ├── data/         # asset paths, animation clip data
│   ├── factories/    # player, enemy, item, bullet, door creation
│   ├── map/          # map data, loader, runtime map, stage manager
│   ├── scenes/       # scene interface / scene implementations
│   └── systems/      # gameplay systems
│
└── vendor/
    └── tinyxml2      # XML parsing for TMX maps
````

---

## Core Engine / Game Responsibilities

### ECS Layer

The ECS layer provides the base architecture for entities, components, and entity management. This is the foundation that gameplay code builds on.

### Map System

The map subsystem is split into dedicated responsibilities:

* `MapLoader` parses TMX files into structured data
* `MapData` stores terrain, spawns, doors, colliders, and metadata
* `Map` wraps loaded data and handles tilemap rendering
* `StageManager` clears old stage entities and spawns the next stage

This design makes level content data-driven rather than hardcoded.

### Systems

Gameplay is divided into dedicated systems such as:

* movement
* combat
* interaction
* collision / physics
* rendering
* animation

The `Game` class mainly coordinates the loop and delegates gameplay work to systems and stage management.

### Factories

Factories are used to create entity archetypes such as:

* player
* enemies
* bullets
* items
* doors

This keeps creation logic consistent and prevents entity setup from being scattered across the codebase.

---

## Map Pipeline

Levels are authored as TMX maps and loaded at runtime.

### Supported map content

A map can define:

* terrain tiles
* named player spawn points
* enemy spawn points
* item spawn points
* collision rectangles
* doors to other stages

`MapLoader` reads named object groups such as spawn, enemy, item, collision, and door layers, which makes the loader more robust than relying on object-group order alone.

### Runtime flow

1. A TMX file is loaded
2. Map data is parsed into `MapData`
3. `StageManager` clears old non-player entities
4. The new map becomes active
5. Doors, enemies, items, and player placement are spawned from the loaded data

This makes stage content extensible and easier to maintain.

---

## Runtime Context

Shared runtime state is stored in `GameContext`, including:

* renderer
* tileset texture
* UI font
* camera
* active map
* player entity reference
* stage transition requests
* pending bullet spawns
* shared timers and counters

This allows systems to interact without tightly coupling everything to the `Game` class.

---

## Current Gameplay Loop

1. Load into a stage
2. Move through the map
3. Fight enemies
4. Collect items
5. Use doors to transition between stages
6. Continue progressing through the game

---

## Controls

Current gameplay input includes:

* **A** — left
* **D** — right
* **AA(hold)** — left dash
* **DD(hold)** - right dash
* **W** — jump
* **Space** — shoot
* **R** — reload

---

## Build Instructions

## Requirements

* C++
* CMake
* SDL3
* SDL3_image
* SDL3_ttf
* tinyxml2

## Build

```bash
mkdir cmake-build-debug
cd cmake-build-debug
cmake ..
make
```

## Run

```bash
./LastCarriage
```

---

## Design Goals

This project was designed to demonstrate:

* a coherent ECS-based engine
* modular systems and reusable components
* extensible game architecture
* clean separation between parsing, runtime state, and gameplay logic
* a complete game implementation built on top of the engine

---

## Notable Design Decisions

### Why ECS?

ECS keeps behavior modular and composable. Instead of putting all behavior into large inheritance trees, game objects are assembled from components and updated by systems.

### Why factories?

Factories centralize entity setup so that players, enemies, bullets, items, and doors are created consistently.

### Why data-driven maps?

Using TMX maps allows stage design to evolve without rewriting game logic. Enemies, items, colliders, and transitions can be changed in the map editor rather than directly in source code.

### Why separate map parsing from runtime map logic?

Separating parsing (`MapLoader`) from runtime access/rendering (`Map`) improves code clarity and makes the architecture easier to extend and debug.

---

## Code Quality Goals

The project emphasizes:

* modularity
* separation of concerns
* readability
* extensibility
* maintainable folder organization
* reusable gameplay architecture

The codebase was structured so that new enemies, items, maps, and systems can be added without rewriting the whole engine.

---

## Future Improvements

Potential future work includes:

* title screen and end screen polish
* additional enemy variety
* stronger UI feedback
* more stages and mechanics
* checkpoint / save system
* improved audio and presentation polish
* expanded boss encounters
* AI-driven enemy logic

