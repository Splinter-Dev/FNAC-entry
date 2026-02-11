#pragma once

#include <raylib.h>
#include <string>
#include <vector>
#include "light.hh"

#define DEFAULT_AMBIENT Vector4{ 0.3f, 0.3f, 0.3f, 1.0f }
#define DEFAULT_VIEW_POS Vector3{ 0.0f, 0.0f, 0.0f }
#define SM ShaderManager::instance()

class ShaderManager {
public:
    // Access the singleton instance
    static ShaderManager& instance();

    Shader& getToon();
    Shader& getBasic();

    // Unload specific shader
    void unload(Shader& shader);

    // Load arbitrary shader by path
    Shader loadCustom(const std::string& vs, const std::string& fs);

    // Send lights to loaded shaders
    void sendLights(const std::vector<Light>& lights);
    void setCamera(const Camera& camera);
    void setAmbient(const Color& color);

    // Cleanup all shaders
    void unloadAll();

    std::string toString() const;

private:

    void initEmptyLights();

    // Private constructor for singleton
    ShaderManager() = default;
    ~ShaderManager();

    // Prevent copies
    ShaderManager(const ShaderManager&) = delete;
    ShaderManager& operator=(const ShaderManager&) = delete;

    // Raylibs default shader
    Shader basic{};
    bool basicLoaded = false;

    // Toon shader
    Shader toon{};
    int toonAmbientLoc;
    int toonViewPosLoc;
    bool toonLoaded = false;
};
