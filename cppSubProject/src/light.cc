#include "light.hh"

Light directionalLight(Vector3 position, Vector3 target, Color color, float intensity) {
    Light light;
    light.enabled = true;
    light.type = LIGHT_DIRECTIONAL;
    light.position = position;
    light.target = target;
    light.color = color;
    light.intensity = intensity;
    return light;
}

Light pointLight(Vector3 position, Color color, float intensity, float radius) {
    Light light;
    light.enabled = true;
    light.type = LIGHT_POINT;
    light.position = position;
    light.color = color;
    light.intensity = intensity;
    light.radius = radius;
    return light;
}   
