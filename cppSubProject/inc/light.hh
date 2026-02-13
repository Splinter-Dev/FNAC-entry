#pragma once

#include <raylib.h>

enum LightType {
    LIGHT_DIRECTIONAL = 0,
    LIGHT_POINT = 1
};

struct Light {
    bool enabled;
    LightType type;      // directional or point
    Vector3 position;
    Vector3 target;      // only for directional lights
    Color color;
    float intensity;
    float radius;        // for point lights
    float specular;
};

Light directionalLight(Vector3 position, Vector3 target, Color color, float intensity);
Light pointLight(Vector3 position, Color color, float intensity, float radius);
Light directionalLight(Vector3 position, Vector3 target, Color color, float intensity, float specular);
Light pointLight(Vector3 position, Color color, float intensity, float radius, float specular);
