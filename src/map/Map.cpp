#include "Map.h"
#include "MapLoader.h"

Map::Map() = default;

bool Map::load(const char* path) {
    MapLoader loader;
    return loader.load(path, data);
}

void Map::clear() {
    data.clear();
}

void Map::render(SDL_Renderer* renderer, SDL_Texture* tilesetTexture, const SDL_FRect& camera) const {
    if (!renderer || !tilesetTexture || data.tileWidth <= 0 || data.tileHeight <= 0 || data.tiles.empty()) {
        return;
    }

    float textureWidth = 0.0f;
    float textureHeight = 0.0f;
    SDL_GetTextureSize(tilesetTexture, &textureWidth, &textureHeight);

    const int tilesPerRow = static_cast<int>(textureWidth) / data.tileWidth;
    if (tilesPerRow <= 0) {
        return;
    }

    for (int row = 0; row < data.height; ++row) {
        for (int col = 0; col < data.width; ++col) {
            const int gid = data.tiles[row][col];
            if (gid == 0) {
                continue;
            }

            const int localTileID = gid - data.firstGid;
            if (localTileID < 0) {
                continue;
            }

            SDL_FRect srcRect{
                static_cast<float>((localTileID % tilesPerRow) * data.tileWidth),
                static_cast<float>((localTileID / tilesPerRow) * data.tileHeight),
                static_cast<float>(data.tileWidth),
                static_cast<float>(data.tileHeight)
            };

            SDL_FRect dstRect{
                static_cast<float>(col * data.tileWidth) - camera.x,
                static_cast<float>(row * data.tileHeight) - camera.y,
                static_cast<float>(data.tileWidth),
                static_cast<float>(data.tileHeight)
            };

            SDL_RenderTexture(renderer, tilesetTexture, &srcRect, &dstRect);
        }
    }
}

const std::vector<NamedSpawnPoint>& Map::getSpawnPoints() const { return data.spawnPoints; }
const std::vector<ItemSpawn>& Map::getItemSpawns() const { return data.itemSpawns; }
const std::vector<EnemySpawn>& Map::getEnemySpawns() const { return data.enemySpawns; }
const std::vector<SDL_FRect>& Map::getColliders() const { return data.colliders; }
const std::vector<DoorSpawn>& Map::getDoors() const { return data.doors; }

bool Map::hasSpawnPoint(const std::string& id) const {
    for (const auto& spawn : data.spawnPoints) {
        if (spawn.id == id) {
            return true;
        }
    }
    return false;
}

NamedSpawnPoint Map::getSpawnPoint(const std::string& id) const {
    for (const auto& spawn : data.spawnPoints) {
        if (spawn.id == id) {
            return spawn;
        }
    }
    return {};
}

int Map::getWidth() const { return data.width; }
int Map::getHeight() const { return data.height; }
int Map::getTileWidth() const { return data.tileWidth; }
int Map::getTileHeight() const { return data.tileHeight; }

const MapData& Map::getData() const { return data; }
MapData& Map::getData() { return data; }