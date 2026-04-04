/**
 * Load and cache textures, fonts
 * Release cached assets on shutdown
 * Hide SDl loading details from factories and game code
 */
#include "AssetManager.h"

#include <iostream>
#include <sstream>

AssetManager::~AssetManager() {
    clear();
}

void AssetManager::setRenderer(SDL_Renderer* rendererPtr) {
    renderer = rendererPtr;
}

SDL_Texture* AssetManager::getTexture(const std::string& path) {
    auto texturePath = textures.find(path);
    if (texturePath != textures.end()) {
        return texturePath->second;
    }

    if (!renderer) {
        std::cout << "AssetManager: renderer is null. Cannot load texture: " << path << '\n';
        return nullptr;
    }

    SDL_Texture* texture = IMG_LoadTexture(renderer, path.c_str());
    if (!texture) {
        std::cout << "AssetManager: failed to load texture: " << path
          << " error=" << SDL_GetError() << '\n';
        return nullptr;
    }

    textures[path] = texture;
    return texture;
}

TTF_Font *AssetManager::getFont(const std::string &path, float size) {
    const std::string key = fontKey(path, size);

    auto it = fonts.find(key);
    if (it != fonts.end()) {
        return it->second;
    }

    TTF_Font* font = TTF_OpenFont(path.c_str(), size);
    if (!font) {
        std::cout << "AssetManager: failed to load font: " << path
          << " size=" << size
          << " error=" << SDL_GetError() << '\n';
        return nullptr;
    }

    fonts[key] = font;
    return font;
}

void AssetManager::clear() {
    for (auto& [path, texture] : textures) {
        if (texture) {
            SDL_DestroyTexture(texture);
        }
    }
    textures.clear();

    for (auto& [key, font] : fonts) {
        if (font) {
            TTF_CloseFont(font);
        }
    }
    fonts.clear();
}

std::string AssetManager::fontKey(const std::string &path, float size) const {
    std::ostringstream oss;
    oss << path << "#" << size;
    return oss.str();
}
