#include "resourceManager.hh"
#include <filesystem>
#include <utility>

// TEMPLATE METAPROGRAMMING
template<typename T, typename Loader>
T* ResourceManager::getOrLoad(
    std::unordered_map<std::string, T>& map,
    std::string_view rawPath,
    Loader loader)
{
    std::filesystem::path p(rawPath);
    p = p.lexically_normal();

    std::string key = p.generic_string();

    auto it = map.find(key);
    if (it != map.end()) {
        return &it->second;
    }

    T resource = loader(key);

    auto [inserted, success] =
        map.emplace(std::move(key), std::move(resource));

    return &inserted->second;
}

Texture2D* ResourceManager::getTexture(std::string_view path) {
    return getOrLoad(textures, path,
        [](const std::string& p) {
            return LoadTexture(p.c_str());
        });
}

Sound* ResourceManager::getSound(std::string_view path) {
    return getOrLoad(sounds, path,
        [](const std::string& p) {
            return LoadSound(p.c_str());
        });
}

Music* ResourceManager::getMusic(std::string_view path) {
    return getOrLoad(musics, path,
        [](const std::string& p) {
            return LoadMusicStream(p.c_str());
        });
}

Font* ResourceManager::getFont(std::string_view path) {
    return getOrLoad(fonts, path,
        [](const std::string& p) {
            return LoadFont(p.c_str());
        });
}

Model* ResourceManager::getModel(std::string_view path) {
    return getOrLoad(models, path,
        [](const std::string& p) {
            return LoadModel(p.c_str());
        });
}

// ----- Unload functions -----

void ResourceManager::unloadTextures() {
    for (auto& [_, tex] : textures) {
        UnloadTexture(tex);
    }
}

void ResourceManager::unloadSounds() {
    for (auto& [_, sound] : sounds) {
        UnloadSound(sound);
    }
}

void ResourceManager::unloadMusics() {
    for (auto& [_, music] : musics) {
        UnloadMusicStream(music);
    }
}

void ResourceManager::unloadFonts() {
    for (auto& [_, font] : fonts) {
        UnloadFont(font);
    }
}

void ResourceManager::unloadModels() {
    for (auto& [_, model] : models) {
        UnloadModel(model);
    }
}

void ResourceManager::unloadAll() {
    unloadTextures();
    unloadSounds();
    unloadMusics();
    unloadFonts();
    unloadModels();
}

ResourceManager::~ResourceManager() {
    unloadAll();
}
