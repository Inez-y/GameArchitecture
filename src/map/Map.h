/**
 * Thinner wrapper
 * Enemy patrol/faving van be map-driven instead of hardcoded patrol
 */
#pragma once

#include <SDL3/SDL.h>
#include <string>
#include <vector>
#include "MapData.h"

class Map {
public:
    Map();

    bool load(const char* path);
    void clear();

    void render(SDL_Renderer* renderer, SDL_Texture* tilesetTexture, const SDL_FRect& camera) const;

    const std::vector<NamedSpawnPoint>& getSpawnPoints() const;
    const std::vector<ItemSpawn>& getItemSpawns() const;
    const std::vector<EnemySpawn>& getEnemySpawns() const;
    const std::vector<SDL_FRect>& getColliders() const;
    const std::vector<DoorSpawn>& getDoors() const;

    bool hasSpawnPoint(const std::string& id) const;
    NamedSpawnPoint getSpawnPoint(const std::string& id) const;

    int getWidth() const;
    int getHeight() const;
    int getTileWidth() const;
    int getTileHeight() const;

    const MapData& getData() const;
    MapData& getData();

private:
    MapData data;
};