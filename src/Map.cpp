#include "Map.h"
#include "../assets/tinyxml2.h"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

Map::Map()
    : width(0),
      height(0),
      tileWidth(0),
      tileHeight(0),
      firstGid(1) {
}

bool Map::load(const char* path) {
    using namespace tinyxml2;

    XMLDocument doc;
    if (doc.LoadFile(path) != XML_SUCCESS) {
        std::cout << "Failed to load map file: " << path << std::endl;
        return false;
    }

    XMLElement* mapNode = doc.FirstChildElement("map");
    if (!mapNode) {
        std::cout << "No <map> element found.\n";
        return false;
    }

    mapNode->QueryIntAttribute("width", &width);
    mapNode->QueryIntAttribute("height", &height);
    mapNode->QueryIntAttribute("tilewidth", &tileWidth);
    mapNode->QueryIntAttribute("tileheight", &tileHeight);

    tiles.clear();
    itemSpawnPoints.clear();
    colliders.clear();

    XMLElement* tilesetNode = mapNode->FirstChildElement("tileset");
    if (tilesetNode) {
        tilesetNode->QueryIntAttribute("firstgid", &firstGid);
    }

    // Find terrain layer by name
    bool terrainLoaded = false;
    XMLElement* layerNode = mapNode->FirstChildElement("layer");
    while (layerNode) {
        const char* layerName = layerNode->Attribute("name");

        if (layerName && std::string(layerName) == "Terrain Layer") {
            XMLElement* dataNode = layerNode->FirstChildElement("data");
            if (!dataNode) {
                std::cout << "Terrain Layer has no data node.\n";
                return false;
            }

            const char* encoding = dataNode->Attribute("encoding");
            if (!encoding || std::string(encoding) != "csv") {
                std::cout << "Only CSV encoding is supported.\n";
                return false;
            }

            const char* csvText = dataNode->GetText();
            if (!csvText) {
                std::cout << "No CSV text found in Terrain Layer.\n";
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

            if (static_cast<int>(flatTiles.size()) != width * height) {
                std::cout << "Tile count mismatch. Expected "
                          << width * height << ", got "
                          << flatTiles.size() << std::endl;
                return false;
            }

            tiles.resize(height, std::vector<int>(width, 0));

            for (int row = 0; row < height; row++) {
                for (int col = 0; col < width; col++) {
                    tiles[row][col] = flatTiles[row * width + col];
                }
            }

            terrainLoaded = true;
            break;
        }

        layerNode = layerNode->NextSiblingElement("layer");
    }

    if (!terrainLoaded) {
        std::cout << "Could not find Terrain Layer.\n";
        return false;
    }

    // Read object groups by name, not by order
    XMLElement* objectGroupNode = mapNode->FirstChildElement("objectgroup");
    while (objectGroupNode) {
        const char* groupName = objectGroupNode->Attribute("name");

        if (groupName) {
            std::string name = groupName;

            if (name == "Item Layer") {
                XMLElement* objectNode = objectGroupNode->FirstChildElement("object");
                while (objectNode) {
                    float x = objectNode->FloatAttribute("x");
                    float y = objectNode->FloatAttribute("y");

                    itemSpawnPoints.push_back({x, y});

                    objectNode = objectNode->NextSiblingElement("object");
                }
            }
            else if (name == "Collision Layer") {
                XMLElement* objectNode = objectGroupNode->FirstChildElement("object");
                while (objectNode) {
                    SDL_FRect rect;
                    rect.x = objectNode->FloatAttribute("x");
                    rect.y = objectNode->FloatAttribute("y");
                    rect.w = objectNode->FloatAttribute("width");
                    rect.h = objectNode->FloatAttribute("height");

                    colliders.push_back(rect);

                    objectNode = objectNode->NextSiblingElement("object");
                }
            }
        }

        objectGroupNode = objectGroupNode->NextSiblingElement("objectgroup");
    }

    return true;
}

void Map::render(SDL_Renderer* renderer, SDL_Texture* tilesetTexture, const SDL_FRect& camera) {
    if (!renderer || !tilesetTexture) {
        return;
    }

    float textureWidth = 0.0f;
    float textureHeight = 0.0f;
    SDL_GetTextureSize(tilesetTexture, &textureWidth, &textureHeight);

    int tilesPerRow = static_cast<int>(textureWidth) / tileWidth;
    if (tilesPerRow <= 0) {
        return;
    }

    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            int gid = tiles[row][col];

            if (gid == 0) {
                continue;
            }

            int localTileID = gid - firstGid;
            if (localTileID < 0) {
                continue;
            }

            SDL_FRect srcRect;
            srcRect.x = static_cast<float>((localTileID % tilesPerRow) * tileWidth);
            srcRect.y = static_cast<float>((localTileID / tilesPerRow) * tileHeight);
            srcRect.w = static_cast<float>(tileWidth);
            srcRect.h = static_cast<float>(tileHeight);

            SDL_FRect dstRect;
            dstRect.x = static_cast<float>(col * tileWidth) - camera.x;
            dstRect.y = static_cast<float>(row * tileHeight) - camera.y;
            dstRect.w = static_cast<float>(tileWidth);
            dstRect.h = static_cast<float>(tileHeight);

            SDL_RenderTexture(renderer, tilesetTexture, &srcRect, &dstRect);
        }
    }
}

const std::vector<SpawnPoint>& Map::getItemSpawnPoints() const {
    return itemSpawnPoints;
}

const std::vector<SDL_FRect>& Map::getColliders() const {
    return colliders;
}

int Map::getWidth() const {
    return width;
}

int Map::getHeight() const {
    return height;
}

int Map::getTileWidth() const {
    return tileWidth;
}

int Map::getTileHeight() const {
    return tileHeight;
}