#include <raylib.h>
#include "shaderManager.hh"
#include "shaders/toon.hh"

static Vector4 defaultAmbient = DEFAULT_AMBIENT;
static Vector3 defaultViewPos = DEFAULT_VIEW_POS;

#define LOG(msg) TraceLog(LOG_INFO, "[ShaderManager] " msg)

// Send a single light to the shader
static void sendLight(Shader &shader, int index, const Light &light) {

    std::string prefix = "lights[" + std::to_string(index) + "].";
    int enabled = light.enabled ? 1 : 0;
    int type = light.type;

    Vector3 intensity = Vector3{ light.intensity, light.intensity, light.intensity };
    Vector3 radius = Vector3{ light.radius, light.radius, light.radius };

    // Convert Color to float
    Vector4 colorF = { 
        light.color.r/255.0f, 
        light.color.g/255.0f, 
        light.color.b/255.0f, 
        light.color.a/255.0f };

    /* Assume the shader has these uniforms */
    SetShaderValue(shader, GetShaderLocation(shader, (prefix + "enabled").c_str()), &enabled, SHADER_UNIFORM_INT);
    SetShaderValue(shader, GetShaderLocation(shader, (prefix + "type").c_str()), &type, SHADER_UNIFORM_INT);
    SetShaderValue(shader, GetShaderLocation(shader, (prefix + "position").c_str()), &light.position, SHADER_UNIFORM_VEC3);
    SetShaderValue(shader, GetShaderLocation(shader, (prefix + "target").c_str()), &light.target, SHADER_UNIFORM_VEC3);
    SetShaderValue(shader, GetShaderLocation(shader, (prefix + "color").c_str()), &colorF, SHADER_UNIFORM_VEC4);
    SetShaderValue(shader, GetShaderLocation(shader, (prefix + "intensity").c_str()), &intensity, SHADER_UNIFORM_VEC3);
    SetShaderValue(shader, GetShaderLocation(shader, (prefix + "radius").c_str()), &radius, SHADER_UNIFORM_VEC3);
}

ShaderManager& ShaderManager::instance() {
    static ShaderManager inst;
    return inst;
}

Shader& ShaderManager::getToon() {
    if (!toonLoaded) {
        toon = loadCustom(toon_vs, toon_fs);
        toonAmbientLoc = GetShaderLocation(toon, "ambient");
        toonViewPosLoc = GetShaderLocation(toon, "viewPos");
        SetShaderValue(toon, toonAmbientLoc, &defaultAmbient, SHADER_UNIFORM_VEC4);
        SetShaderValue(toon, toonViewPosLoc, &defaultViewPos, SHADER_UNIFORM_VEC3);
        toonLoaded = true;
        LOG("Loaded toon shader");
    }
    return toon;
}

Shader& ShaderManager::getBasic() {
    if (!basicLoaded) {
        basic = LoadShader(nullptr, nullptr);   // default raylib shader
        basicLoaded = true;
        LOG("Loaded basic shader");
    }
    return basic;
}

Shader ShaderManager::loadCustom(const std::string& vs, const std::string& fs) {
    return LoadShaderFromMemory(vs.c_str(), fs.c_str());
}

void ShaderManager::sendLights(const std::vector<Light>& lights) {
    for (int i = 0; i < (int)lights.size(); i++) {
        sendLight(toon, i, lights[i]);
    }
}

void ShaderManager::setCamera(const Camera& camera) {
    if (toonLoaded) {
        SetShaderValue(toon, toonViewPosLoc, 
            &camera.position, SHADER_UNIFORM_VEC3);
    }
    if (basicLoaded) {
        // Notin to do ig
    }
}

void ShaderManager::unloadAll() {
    if (toonLoaded)  UnloadShader(toon);
    if (basicLoaded) UnloadShader(basic);
    toonLoaded = false;
    basicLoaded = false;
}

ShaderManager::~ShaderManager() {
    unloadAll();
}
