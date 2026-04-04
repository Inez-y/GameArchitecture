/**
 * Holds map content
 */
#ifndef LASTCARRIAGE_MAPDATA_H
#define LASTCARRIAGE_MAPDATA_H
#pragma once

#include <SDL3/SDL.h>
#include <string>
#include <vector>

struct NamedSpawnPoint {
    std::string id;
    float x = 0.0f;
    float y = 0.0f;
};

struct ItemSpawn {
    float x = 0.0f;
    float y = 0.0f;
    std::string type;
};

struct EnemySpawn {
    float x = 0.0f;
    float y = 0.0f;
    std::string type;

    float patrolLeft = 0.0f;
    float patrolRight = 0.0f;
    int facing = 1;
};

struct DoorSpawn {
    float x = 0.0f;
    float y = 0.0f;
    float w = 0.0f;
    float h = 0.0f;
    std::string targetMap;
    std::string targetSpawn;
};

struct MapData {
    std::vector<std::vector<int>> tiles;
    std::vector<NamedSpawnPoint> spawnPoints;
    std::vector<ItemSpawn> itemSpawns;
    std::vector<EnemySpawn> enemySpawns;
    std::vector<SDL_FRect> colliders;
    std::vector<DoorSpawn> doors;

    int width = 0;
    int height = 0;
    int tileWidth = 0;
    int tileHeight = 0;
    int firstGid = 1;

    void clear() {
        tiles.clear();
        spawnPoints.clear();
        itemSpawns.clear();
        enemySpawns.clear();
        colliders.clear();
        doors.clear();

        width = 0;
        height = 0;
        tileWidth = 0;
        tileHeight = 0;
        firstGid = 1;
    }
};

#endif //LASTCARRIAGE_MAPDATA_H