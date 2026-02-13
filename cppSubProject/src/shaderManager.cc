#include <raylib.h>
#include "shaderManager.hh"
#include "shaders/toon.hh"

static Vector4 defaultAmbient = DEFAULT_AMBIENT;
static Vector3 defaultViewPos = DEFAULT_VIEW_POS;

#define LOG(msg) TraceLog(LOG_INFO, "[ShaderManager] " msg)

#define MAX_LIGHTS 10

// Send a single light to the shader
static void sendLight(Shader &shader, int index, const Light &light) {

    std::string prefix = "lights[" + std::to_string(index) + "].";
    int enabled = light.enabled ? 1 : 0;
    int type = light.type;

    Vector3 intensity = Vector3 { light.intensity, light.intensity, light.intensity };
    Vector3 radius = Vector3 { light.radius, light.radius, light.radius };

    // Convert Color to float
    Vector4 colorF = { 
        light.color.r/255.0f, 
        light.color.g/255.0f, 
        light.color.b/255.0f, 
        light.color.a/255.0f 
    };

    int enabledLoc = GetShaderLocation(shader, (prefix + "enabled").c_str());
    int typeLoc = GetShaderLocation(shader, (prefix + "type").c_str());
    int positionLoc = GetShaderLocation(shader, (prefix + "position").c_str());
    int targetLoc = GetShaderLocation(shader, (prefix + "target").c_str());
    int colorLoc = GetShaderLocation(shader, (prefix + "color").c_str());
    int intensityLoc = GetShaderLocation(shader, (prefix + "intensity").c_str());
    int radiusLoc = GetShaderLocation(shader, (prefix + "radius").c_str());
    int specularLoc = GetShaderLocation(shader, (prefix + "specular").c_str());

    /* Assume the shader has these uniforms */
    SetShaderValue(shader, enabledLoc, &enabled, SHADER_UNIFORM_INT);
    SetShaderValue(shader, typeLoc, &type, SHADER_UNIFORM_INT);
    SetShaderValue(shader, positionLoc, &light.position, SHADER_UNIFORM_VEC3);
    SetShaderValue(shader, targetLoc, &light.target, SHADER_UNIFORM_VEC3);
    SetShaderValue(shader, colorLoc, &colorF, SHADER_UNIFORM_VEC4);
    SetShaderValue(shader, intensityLoc, &intensity, SHADER_UNIFORM_VEC3);
    SetShaderValue(shader, radiusLoc, &radius, SHADER_UNIFORM_VEC3);
    SetShaderValue(shader, specularLoc, &light.specular, SHADER_UNIFORM_FLOAT);

    // Log if any of the uniforms were not found
    if (enabledLoc == -1)   LOG("Could not find enabled uniform");
    if (typeLoc == -1)      LOG("Could not find type uniform");
    if (positionLoc == -1)  LOG("Could not find position uniform");
    if (targetLoc == -1)    LOG("Could not find target uniform");
    if (colorLoc == -1)     LOG("Could not find color uniform");
    if (intensityLoc == -1) LOG("Could not find intensity uniform");
    if (radiusLoc == -1)    LOG("Could not find radius uniform");
    if (specularLoc == -1)  LOG("Could not find specular uniform");
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
        basic = LoadShader(nullptr, nullptr); // default raylib shader
        basicLoaded = true;
        LOG("Loaded basic shader");
    }
    return basic;
}

Shader ShaderManager::loadCustom(const std::string& vs, const std::string& fs) {
    return LoadShaderFromMemory(vs.c_str(), fs.c_str());
}

void ShaderManager::sendLights(const std::vector<Light>& lights) {
    int i = 0;

    if (lights.size() > MAX_LIGHTS)
        TraceLog(LOG_WARNING, "Too many lights, only sending %d", MAX_LIGHTS);

    if (toonLoaded) {
        for (i = 0; i < (int)lights.size() && i < MAX_LIGHTS; i++)
            sendLight(toon, i, lights[i]);
        for (; i < MAX_LIGHTS; i++)
            sendLight(toon, i, Light{ false, LIGHT_DIRECTIONAL, Vector3{ 0.0f, 0.0f, 0.0f }, Vector3{ 0.0f, 0.0f, 0.0f }, WHITE, 0.0f, 0.0f });
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

void ShaderManager::setAmbient(const Color& color) {
    Vector4 colorF = { 
        color.r/255.0f, 
        color.g/255.0f, 
        color.b/255.0f, 
        color.a/255.0f 
    };
    if (toonLoaded) {
        SetShaderValue(toon, toonAmbientLoc, 
            &colorF, SHADER_UNIFORM_VEC4);
    }
}

void ShaderManager::initEmptyLights() {
    std::vector<Light> lights;
    sendLights(lights);
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
