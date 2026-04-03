#pragma once

#include <SDL3/SDL.h>
#include <string>
#include <vector>
#include "../vendor/tinyxml2.h"

struct SpawnPoint {
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
};

struct DoorSpawn {
    float x = 0.0f;
    float y = 0.0f;
    float w = 0.0f;
    float h = 0.0f;
    std::string targetMap;
};

class Map {
public:
    Map();

    bool load(const char* path);
    void clear();

    void render(SDL_Renderer* renderer, SDL_Texture* tilesetTexture, const SDL_FRect& camera) const;

    const std::vector<ItemSpawn>& getItemSpawns() const;
    const std::vector<EnemySpawn>& getEnemySpawns() const;
    const std::vector<SDL_FRect>& getColliders() const;
    const std::vector<DoorSpawn>& getDoors() const;

    SpawnPoint getPlayerSpawn() const;
    bool hasPlayerSpawn() const;

    int getWidth() const;
    int getHeight() const;
    int getTileWidth() const;
    int getTileHeight() const;

private:
    bool parseTerrainLayer(class tinyxml2::XMLElement* mapNode);
    void parseObjectGroups(class tinyxml2::XMLElement* mapNode);

    void parseSpawnLayer(class tinyxml2::XMLElement* objectGroupNode);
    void parseEnemySpawnLayer(class tinyxml2::XMLElement* objectGroupNode);
    void parseItemLayer(class tinyxml2::XMLElement* objectGroupNode);
    void parseCollisionLayer(class tinyxml2::XMLElement* objectGroupNode);
    void parseDoorLayer(class tinyxml2::XMLElement* objectGroupNode);

    std::string getPropertyString(class tinyxml2::XMLElement* objectNode, const char* propertyName) const;

private:
    std::vector<std::vector<int>> tiles;
    std::vector<ItemSpawn> itemSpawns;
    std::vector<EnemySpawn> enemySpawns;
    std::vector<SDL_FRect> colliders;
    std::vector<DoorSpawn> doors;

    SpawnPoint playerSpawn{};
    bool playerSpawnSet = false;

    int width = 0;
    int height = 0;
    int tileWidth = 0;
    int tileHeight = 0;
    int firstGid = 1;
};