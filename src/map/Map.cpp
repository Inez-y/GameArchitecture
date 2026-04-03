#include "Map.h"
#include "../vendor/tinyxml2.h"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace tinyxml2;

Map::Map() = default;

void Map::clear() {
    tiles.clear();
    itemSpawns.clear();
    enemySpawns.clear();
    colliders.clear();
    doors.clear();

    playerSpawn = {};
    playerSpawnSet = false;

    width = 0;
    height = 0;
    tileWidth = 0;
    tileHeight = 0;
    firstGid = 1;
}

bool Map::load(const char* path) {
    clear();

    if (!path || path[0] == '\0') {
        std::cout << "Map::load received empty path.\n";
        return false;
    }

    XMLDocument doc;
    const XMLError loadResult = doc.LoadFile(path);
    if (loadResult != XML_SUCCESS) {
        std::cout << "Failed to load map file: [" << path << "] error=" << loadResult << '\n';
        return false;
    }

    XMLElement* mapNode = doc.FirstChildElement("map");
    if (!mapNode) {
        std::cout << "No <map> element found.\n";
        return false;
    }

    if (mapNode->QueryIntAttribute("width", &width) != XML_SUCCESS ||
        mapNode->QueryIntAttribute("height", &height) != XML_SUCCESS ||
        mapNode->QueryIntAttribute("tilewidth", &tileWidth) != XML_SUCCESS ||
        mapNode->QueryIntAttribute("tileheight", &tileHeight) != XML_SUCCESS) {
        std::cout << "Map is missing required size attributes.\n";
        return false;
    }

    XMLElement* tilesetNode = mapNode->FirstChildElement("tileset");
    if (tilesetNode) {
        tilesetNode->QueryIntAttribute("firstgid", &firstGid);
    }

    if (!parseTerrainLayer(mapNode)) {
        return false;
    }

    parseObjectGroups(mapNode);
    return true;
}

bool Map::parseTerrainLayer(XMLElement* mapNode) {
    XMLElement* layerNode = mapNode->FirstChildElement("layer");
    while (layerNode) {
        const char* layerName = layerNode->Attribute("name");
        if (layerName && std::string(layerName) == "Terrain Layer") {
            int layerWidth = 0;
            int layerHeight = 0;
            layerNode->QueryIntAttribute("width", &layerWidth);
            layerNode->QueryIntAttribute("height", &layerHeight);

            if (layerWidth <= 0 || layerHeight <= 0) {
                std::cout << "Terrain Layer has invalid width/height.\n";
                return false;
            }

            // Keep map dimensions aligned with actual terrain layer.
            width = layerWidth;
            height = layerHeight;

            XMLElement* dataNode = layerNode->FirstChildElement("data");
            if (!dataNode) {
                std::cout << "Terrain Layer has no <data> node.\n";
                return false;
            }

            const char* encoding = dataNode->Attribute("encoding");
            if (!encoding || std::string(encoding) != "csv") {
                std::cout << "Only CSV terrain data is supported.\n";
                return false;
            }

            const char* csvText = dataNode->GetText();
            if (!csvText) {
                std::cout << "Terrain Layer CSV text is empty.\n";
                return false;
            }

            std::stringstream ss(csvText);
            std::string token;
            std::vector<int> flatTiles;

            while (std::getline(ss, token, ',')) {
                std::stringstream tokenStream(token);
                int gid = 0;
                if (tokenStream >> gid) {
                    flatTiles.push_back(gid);
                }
            }

            const int expectedTileCount = width * height;
            if (static_cast<int>(flatTiles.size()) != expectedTileCount) {
                std::cout << "Tile count mismatch. Expected "
                          << expectedTileCount
                          << ", got "
                          << flatTiles.size()
                          << '\n';
                return false;
            }

            tiles.assign(height, std::vector<int>(width, 0));
            for (int row = 0; row < height; ++row) {
                for (int col = 0; col < width; ++col) {
                    tiles[row][col] = flatTiles[row * width + col];
                }
            }

            return true;
        }

        layerNode = layerNode->NextSiblingElement("layer");
    }

    std::cout << "Could not find Terrain Layer.\n";
    return false;
}

void Map::parseObjectGroups(XMLElement* mapNode) {
    XMLElement* objectGroupNode = mapNode->FirstChildElement("objectgroup");
    while (objectGroupNode) {
        const char* groupName = objectGroupNode->Attribute("name");
        const std::string name = groupName ? groupName : "";

        if (name == "Spawn Layer") {
            parseSpawnLayer(objectGroupNode);
        } else if (name == "Enemy Spawn Layer") {
            parseEnemySpawnLayer(objectGroupNode);
        } else if (name == "Item Layer") {
            parseItemLayer(objectGroupNode);
        } else if (name == "Collision Layer") {
            parseCollisionLayer(objectGroupNode);
        } else if (name == "Door Layer") {
            parseDoorLayer(objectGroupNode);
        }

        objectGroupNode = objectGroupNode->NextSiblingElement("objectgroup");
    }
}

void Map::parseSpawnLayer(XMLElement* objectGroupNode) {
    XMLElement* objectNode = objectGroupNode->FirstChildElement("object");
    while (objectNode) {
        const char* objectName = objectNode->Attribute("name");
        if (objectName && std::string(objectName) == "PlayerSpawn") {
            playerSpawn.x = objectNode->FloatAttribute("x");
            playerSpawn.y = objectNode->FloatAttribute("y");
            playerSpawnSet = true;
            return;
        }
        objectNode = objectNode->NextSiblingElement("object");
    }
}

void Map::parseEnemySpawnLayer(XMLElement* objectGroupNode) {
    XMLElement* objectNode = objectGroupNode->FirstChildElement("object");
    while (objectNode) {
        EnemySpawn spawn;
        spawn.x = objectNode->FloatAttribute("x");
        spawn.y = objectNode->FloatAttribute("y");

        const char* objectName = objectNode->Attribute("name");
        spawn.type = objectName ? objectName : "Patrol";

        enemySpawns.push_back(spawn);
        objectNode = objectNode->NextSiblingElement("object");
    }
}

void Map::parseItemLayer(XMLElement* objectGroupNode) {
    XMLElement* objectNode = objectGroupNode->FirstChildElement("object");
    while (objectNode) {
        ItemSpawn spawn;
        spawn.x = objectNode->FloatAttribute("x");
        spawn.y = objectNode->FloatAttribute("y");

        const char* objectName = objectNode->Attribute("name");
        spawn.type = objectName ? objectName : "Coin";

        itemSpawns.push_back(spawn);
        objectNode = objectNode->NextSiblingElement("object");
    }
}

void Map::parseCollisionLayer(XMLElement* objectGroupNode) {
    XMLElement* objectNode = objectGroupNode->FirstChildElement("object");
    while (objectNode) {
        SDL_FRect rect{};
        rect.x = objectNode->FloatAttribute("x");
        rect.y = objectNode->FloatAttribute("y");
        rect.w = objectNode->FloatAttribute("width");
        rect.h = objectNode->FloatAttribute("height");

        colliders.push_back(rect);
        objectNode = objectNode->NextSiblingElement("object");
    }
}

void Map::parseDoorLayer(XMLElement* objectGroupNode) {
    XMLElement* objectNode = objectGroupNode->FirstChildElement("object");
    while (objectNode) {
        DoorSpawn door;
        door.x = objectNode->FloatAttribute("x");
        door.y = objectNode->FloatAttribute("y");
        door.w = objectNode->FloatAttribute("width");
        door.h = objectNode->FloatAttribute("height");

        door.targetMap.clear();

        XMLElement* propertiesNode = objectNode->FirstChildElement("properties");
        if (propertiesNode) {
            XMLElement* propertyNode = propertiesNode->FirstChildElement("property");
            while (propertyNode) {
                const char* propertyName = propertyNode->Attribute("name");
                if (propertyName && std::string(propertyName) == "targetMap") {
                    const char* value = propertyNode->Attribute("value");
                    if (value) {
                        door.targetMap = value;
                    } else if (propertyNode->GetText()) {
                        door.targetMap = propertyNode->GetText();
                    }
                    break;
                }
                propertyNode = propertyNode->NextSiblingElement("property");
            }
        }

        // fallback for old maps that stored path in object name
        if (door.targetMap.empty()) {
            const char* objectName = objectNode->Attribute("name");
            if (objectName) {
                door.targetMap = objectName;
            }
        }

        std::cout << "Parsed door targetMap = [" << door.targetMap << "]" << std::endl;
        doors.push_back(door);

        objectNode = objectNode->NextSiblingElement("object");
    }
}

std::string Map::getPropertyString(XMLElement* objectNode, const char* propertyName) const {
    if (!objectNode || !propertyName) {
        return "";
    }

    XMLElement* propertiesNode = objectNode->FirstChildElement("properties");
    if (!propertiesNode) {
        return "";
    }

    XMLElement* propertyNode = propertiesNode->FirstChildElement("property");
    while (propertyNode) {
        const char* name = propertyNode->Attribute("name");
        if (name && std::string(name) == propertyName) {
            const char* value = propertyNode->Attribute("value");
            if (value) {
                return value;
            }

            const char* text = propertyNode->GetText();
            return text ? text : "";
        }

        propertyNode = propertyNode->NextSiblingElement("property");
    }

    return "";
}

void Map::render(SDL_Renderer* renderer, SDL_Texture* tilesetTexture, const SDL_FRect& camera) const {
    if (!renderer || !tilesetTexture || tileWidth <= 0 || tileHeight <= 0 || tiles.empty()) {
        return;
    }

    float textureWidth = 0.0f;
    float textureHeight = 0.0f;
    SDL_GetTextureSize(tilesetTexture, &textureWidth, &textureHeight);

    const int tilesPerRow = static_cast<int>(textureWidth) / tileWidth;
    if (tilesPerRow <= 0) {
        return;
    }

    for (int row = 0; row < height; ++row) {
        for (int col = 0; col < width; ++col) {
            const int gid = tiles[row][col];
            if (gid == 0) {
                continue;
            }

            const int localTileID = gid - firstGid;
            if (localTileID < 0) {
                continue;
            }

            SDL_FRect srcRect{
                static_cast<float>((localTileID % tilesPerRow) * tileWidth),
                static_cast<float>((localTileID / tilesPerRow) * tileHeight),
                static_cast<float>(tileWidth),
                static_cast<float>(tileHeight)
            };

            SDL_FRect dstRect{
                static_cast<float>(col * tileWidth) - camera.x,
                static_cast<float>(row * tileHeight) - camera.y,
                static_cast<float>(tileWidth),
                static_cast<float>(tileHeight)
            };

            SDL_RenderTexture(renderer, tilesetTexture, &srcRect, &dstRect);
        }
    }
}

const std::vector<ItemSpawn>& Map::getItemSpawns() const { return itemSpawns; }
const std::vector<EnemySpawn>& Map::getEnemySpawns() const { return enemySpawns; }
const std::vector<SDL_FRect>& Map::getColliders() const { return colliders; }
const std::vector<DoorSpawn>& Map::getDoors() const { return doors; }

SpawnPoint Map::getPlayerSpawn() const { return playerSpawn; }
bool Map::hasPlayerSpawn() const { return playerSpawnSet; }

int Map::getWidth() const { return width; }
int Map::getHeight() const { return height; }
int Map::getTileWidth() const { return tileWidth; }
int Map::getTileHeight() const { return tileHeight; }