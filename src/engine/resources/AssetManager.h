/**
 * cache textures
 * return already loaded assets instead or reloading
 * hide SDL loading details from gameplay code
 */

#ifndef LASTCARRIAGE_ASSETMANAGER_H
#define LASTCARRIAGE_ASSETMANAGER_H

#pragma once

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <string>
#include <unordered_map>

class AssetManager {
public:
    AssetManager() = default;
    ~AssetManager();

    void setRenderer(SDL_Renderer* renderer);

    SDL_Texture* getTexture(const std::string& path);
    TTF_Font* getFont(const std::string& path, float size);

    void clear();

private:
    SDL_Renderer* renderer = nullptr;

    std::unordered_map<std::string, SDL_Texture*> textures;
    std::unordered_map<std::string, TTF_Font*> fonts;

    std::string fontKey(const std::string& path, float size) const;
};

#endif //LASTCARRIAGE_ASSETMANAGER_H