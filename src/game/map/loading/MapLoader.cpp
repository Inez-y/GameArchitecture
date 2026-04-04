#include "MapLoader.h"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>

using namespace tinyxml2;

bool MapLoader::load(const char* path, MapData& outData) {
    outData.clear();

    if (!path || path[0] == '\0') {
        std::cout << "MapLoader::load received empty path.\n";
        return false;
    }

    XMLDocument doc;
    const XMLError loadResult = doc.LoadFile(path);
    if (loadResult != XML_SUCCESS) {
        std::cout << "Failed to load map file: [" << path << "] error=" << loadResult << '\n';
        std::cout << "TinyXML2 error line: " << doc.ErrorLineNum() << '\n';
        std::cout << "TinyXML2 error text: " << doc.ErrorStr() << '\n';
        doc.PrintError();
        return false;
    }

    XMLElement* mapNode = doc.FirstChildElement("map");
    if (!mapNode) {
        std::cout << "No <map> element found.\n";
        return false;
    }

    if (mapNode->QueryIntAttribute("width", &outData.width) != XML_SUCCESS ||
        mapNode->QueryIntAttribute("height", &outData.height) != XML_SUCCESS ||
        mapNode->QueryIntAttribute("tilewidth", &outData.tileWidth) != XML_SUCCESS ||
        mapNode->QueryIntAttribute("tileheight", &outData.tileHeight) != XML_SUCCESS) {
        std::cout << "Map is missing required size attributes.\n";
        return false;
    }

    XMLElement* tilesetNode = mapNode->FirstChildElement("tileset");
    if (tilesetNode) {
        tilesetNode->QueryIntAttribute("firstgid", &outData.firstGid);
    }

    if (!parseTerrainLayer(mapNode, outData)) {
        return false;
    }

    parseObjectGroups(mapNode, outData);
    return true;
}

bool MapLoader::parseTerrainLayer(XMLElement* mapNode, MapData& outData) {
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

            outData.width = layerWidth;
            outData.height = layerHeight;

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

            const int expectedTileCount = outData.width * outData.height;
            if (static_cast<int>(flatTiles.size()) != expectedTileCount) {
                std::cout << "Tile count mismatch. Expected "
                          << expectedTileCount
                          << ", got "
                          << flatTiles.size()
                          << '\n';
                return false;
            }

            outData.tiles.assign(outData.height, std::vector<int>(outData.width, 0));
            for (int row = 0; row < outData.height; ++row) {
                for (int col = 0; col < outData.width; ++col) {
                    outData.tiles[row][col] = flatTiles[row * outData.width + col];
                }
            }

            return true;
        }

        layerNode = layerNode->NextSiblingElement("layer");
    }

    std::cout << "Could not find Terrain Layer.\n";
    return false;
}

void MapLoader::parseObjectGroups(XMLElement* mapNode, MapData& outData) {
    XMLElement* objectGroupNode = mapNode->FirstChildElement("objectgroup");
    while (objectGroupNode) {
        const char* groupName = objectGroupNode->Attribute("name");
        const std::string name = groupName ? groupName : "";

        if (name == "Spawn Layer") {
            parseSpawnLayer(objectGroupNode, outData);
        } else if (name == "Enemy Spawn Layer") {
            parseEnemySpawnLayer(objectGroupNode, outData);
        } else if (name == "Item Layer") {
            parseItemLayer(objectGroupNode, outData);
        } else if (name == "Collision Layer") {
            parseCollisionLayer(objectGroupNode, outData);
        } else if (name == "Door Layer") {
            parseDoorLayer(objectGroupNode, outData);
        }

        objectGroupNode = objectGroupNode->NextSiblingElement("objectgroup");
    }
}

void MapLoader::parseSpawnLayer(XMLElement* objectGroupNode, MapData& outData) {
    XMLElement* objectNode = objectGroupNode->FirstChildElement("object");
    while (objectNode) {
        const char* objectName = objectNode->Attribute("name");
        if (objectName && std::string(objectName) == "SpawnPoint") {
            NamedSpawnPoint spawn;
            spawn.x = objectNode->FloatAttribute("x");
            spawn.y = objectNode->FloatAttribute("y");
            spawn.id = getPropertyString(objectNode, "id");

            if (spawn.id.empty()) {
                spawn.id = "default";
            }

            outData.spawnPoints.push_back(spawn);
        }

        objectNode = objectNode->NextSiblingElement("object");
    }
}

void MapLoader::parseEnemySpawnLayer(XMLElement* objectGroupNode, MapData& outData) {
    XMLElement* objectNode = objectGroupNode->FirstChildElement("object");
    while (objectNode) {
        EnemySpawn spawn;
        spawn.x = objectNode->FloatAttribute("x");
        spawn.y = objectNode->FloatAttribute("y");

        const char* objectName = objectNode->Attribute("name");
        spawn.type = objectName ? objectName : "Patrol";

        spawn.patrolLeft = getPropertyFloat(objectNode, "patrolLeft", spawn.x - 100.0f);
        spawn.patrolRight = getPropertyFloat(objectNode, "patrolRight", spawn.x + 100.0f);
        spawn.facing = getPropertyInt(objectNode, "facing", 1);

        if (spawn.facing != -1 && spawn.facing != 1) {
            spawn.facing = 1;
        }

        outData.enemySpawns.push_back(spawn);
        objectNode = objectNode->NextSiblingElement("object");
    }
}

void MapLoader::parseItemLayer(XMLElement* objectGroupNode, MapData& outData) {
    XMLElement* objectNode = objectGroupNode->FirstChildElement("object");
    while (objectNode) {
        ItemSpawn spawn;
        spawn.x = objectNode->FloatAttribute("x");
        spawn.y = objectNode->FloatAttribute("y");

        const char* objectName = objectNode->Attribute("name");
        spawn.type = objectName ? objectName : "Coin";

        outData.itemSpawns.push_back(spawn);
        objectNode = objectNode->NextSiblingElement("object");
    }
}

void MapLoader::parseCollisionLayer(XMLElement* objectGroupNode, MapData& outData) {
    XMLElement* objectNode = objectGroupNode->FirstChildElement("object");
    while (objectNode) {
        SDL_FRect rect{};
        rect.x = objectNode->FloatAttribute("x");
        rect.y = objectNode->FloatAttribute("y");
        rect.w = objectNode->FloatAttribute("width");
        rect.h = objectNode->FloatAttribute("height");

        outData.colliders.push_back(rect);
        objectNode = objectNode->NextSiblingElement("object");
    }
}

void MapLoader::parseDoorLayer(XMLElement* objectGroupNode, MapData& outData) {
    XMLElement* objectNode = objectGroupNode->FirstChildElement("object");
    while (objectNode) {
        DoorSpawn door;
        door.x = objectNode->FloatAttribute("x");
        door.y = objectNode->FloatAttribute("y");
        door.w = objectNode->FloatAttribute("width");
        door.h = objectNode->FloatAttribute("height");

        door.targetMap = getPropertyString(objectNode, "targetMap");
        door.targetSpawn = getPropertyString(objectNode, "targetSpawn");

        if (door.targetMap.empty()) {
            const char* objectName = objectNode->Attribute("name");
            door.targetMap = objectName ? trim(objectName) : "";
        }

        if (door.targetSpawn.empty()) {
            door.targetSpawn = "default";
        }

        std::cout << "Loaded door targetMap=[" << door.targetMap
                  << "] targetSpawn=[" << door.targetSpawn << "]\n";

        outData.doors.push_back(door);
        objectNode = objectNode->NextSiblingElement("object");
    }
}

std::string MapLoader::getPropertyString(XMLElement* objectNode, const char* propertyName) const {
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
                return trim(value);
            }

            const char* text = propertyNode->GetText();
            return text ? trim(text) : "";
        }

        propertyNode = propertyNode->NextSiblingElement("property");
    }

    return "";
}

float MapLoader::getPropertyFloat(XMLElement* objectNode, const char* propertyName, float fallback) const {
    const std::string value = getPropertyString(objectNode, propertyName);
    if (value.empty()) {
        return fallback;
    }

    try {
        return std::stof(value);
    } catch (...) {
        return fallback;
    }
}

int MapLoader::getPropertyInt(XMLElement* objectNode, const char* propertyName, int fallback) const {
    const std::string value = getPropertyString(objectNode, propertyName);
    if (value.empty()) {
        return fallback;
    }

    try {
        return std::stoi(value);
    } catch (...) {
        return fallback;
    }
}

std::string MapLoader::trim(const std::string& value) const {
    size_t start = 0;
    while (start < value.size() &&
           std::isspace(static_cast<unsigned char>(value[start]))) {
        ++start;
    }

    size_t end = value.size();
    while (end > start &&
           std::isspace(static_cast<unsigned char>(value[end - 1]))) {
        --end;
    }

    return value.substr(start, end - start);
}
