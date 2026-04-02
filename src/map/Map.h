#pragma once

#include <SDL3/SDL.h>
#include <string>
#include <vector>

#include "../DoorSpawn.h"

// Items
struct SpawnPoint {
    float x;
    float y;
};

struct ItemSpawn {
    float x;
    float y;
    std::string type;
};

struct EnemySpawn {
    float x;
    float y;
    std::string type;
};

struct DoorSpaw {
    float x;
    float y;
    float w;
    float h;
    std::string targetMap;
};

class Map {
public:
    Map();

    bool load(const char* path);
    void render(SDL_Renderer* renderer, SDL_Texture* tilesetTexture, const SDL_FRect& camera);

    const std::vector<ItemSpawn>& getItemSpawns() const;
    const std::vector<EnemySpawn>& getEnemySpawns() const;
    const std::vector<SDL_FRect>& getColliders() const;

    SpawnPoint getPlayerSpawn() const;
    bool hasPlayerSpawn() const;

    int getWidth() const;
    int getHeight() const;
    int getTileWidth() const;
    int getTileHeight() const;

    // To the next map
    const std::vector<DoorSpawn>& getDoors() const;


private:
    std::vector<std::vector<int>> tiles;
    std::vector<ItemSpawn> itemSpawns;
    // std::vector<SpawnPoint> enemySpawnPoints;
    std::vector<EnemySpawn> enemySpawns;
    std::vector<SDL_FRect> colliders;
    std::vector<DoorSpawn> doors;

    SpawnPoint playerSpawn;
    bool playerSpawnSet;

    int width;
    int height;
    int tileWidth;
    int tileHeight;
    int firstGid;
};