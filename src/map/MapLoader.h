/**
 * Parses TMX into data
 */
#ifndef LASTCARRIAGE_MAPLOADER_H
#define LASTCARRIAGE_MAPLOADER_H

#pragma once

#include <string>
#include "../vendor/tinyxml2.h"
#include "MapData.h"

class MapLoader {
public:
    bool load(const char* path, MapData& outData);

private:
    bool parseTerrainLayer(tinyxml2::XMLElement* mapNode, MapData& outData);
    void parseObjectGroups(tinyxml2::XMLElement* mapNode, MapData& outData);

    void parseSpawnLayer(tinyxml2::XMLElement* objectGroupNode, MapData& outData);
    void parseEnemySpawnLayer(tinyxml2::XMLElement* objectGroupNode, MapData& outData);
    void parseItemLayer(tinyxml2::XMLElement* objectGroupNode, MapData& outData);
    void parseCollisionLayer(tinyxml2::XMLElement* objectGroupNode, MapData& outData);
    void parseDoorLayer(tinyxml2::XMLElement* objectGroupNode, MapData& outData);

    std::string getPropertyString(tinyxml2::XMLElement* objectNode, const char* propertyName) const;
    float getPropertyFloat(tinyxml2::XMLElement* objectNode, const char* propertyName, float fallback) const;
    int getPropertyInt(tinyxml2::XMLElement* objectNode, const char* propertyName, int fallback) const;

    std::string trim(const std::string& value) const;
};

#endif // LASTCARRIAGE_MAPLOADER_H