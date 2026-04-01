// The door connects maps
#ifndef LASTCARRIAGE_DOORSPAWN_H
#define LASTCARRIAGE_DOORSPAWN_H
#include <string>

struct DoorSpawn {
    float x;
    float y;
    float w;
    float h;
    std::string targetMap;
};
#endif //LASTCARRIAGE_DOORSPAWN_H