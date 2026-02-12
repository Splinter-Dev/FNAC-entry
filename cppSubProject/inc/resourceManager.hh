#pragma once

#include <unordered_map>
#include <string>
#include <string_view>
#include <raylib.h>

#define RM ResourceManager::getInstance()

class ResourceManager {
public:

    static ResourceManager& getInstance() {
        static ResourceManager instance;
        return instance;
    }

    Texture2D*  getTexture(std::string_view path);
    Sound*      getSound(std::string_view path);
    Music*      getMusic(std::string_view path);
    Font*       getFont(std::string_view path);
    Model*      getModel(std::string_view path);

    void unloadAll();
    void unloadTextures();
    void unloadSounds();
    void unloadMusics();
    void unloadFonts();
    void unloadModels();

    ~ResourceManager();

private:
    ResourceManager() = default;

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    template<typename T, typename Loader>
    T* getOrLoad(std::unordered_map<std::string, T>& map,
                 std::string_view path,
                 Loader loader);

    std::unordered_map<std::string, Texture2D> textures;
    std::unordered_map<std::string, Sound>     sounds;
    std::unordered_map<std::string, Music>     musics;
    std::unordered_map<std::string, Font>      fonts;
    std::unordered_map<std::string, Model>     models;
};
