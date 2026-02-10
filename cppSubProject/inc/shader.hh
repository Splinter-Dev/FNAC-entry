/* Here we are using a toon shader */

#pragma once

extern const char * toon_vs;
extern const char * toon_fs;

#include <raylib.h>
#include <vector>
#include "light.hh"

extern Shader toonShader;
extern int ambientLoc;
extern int viewPosLoc;

#define DEFAULT_AMBIENT (Vector4){ 0.3f, 0.3f, 0.3f, 1.0f }
#define DEFAULT_VIEW_POS (Vector3){ 0.0f, 0.0f, 0.0f }
#define DEFAULT_LIGHTS (Light){ 1, LIGHT_DIRECTIONAL, (Vector3){ 0.0f, 0.0f, 0.0f }, (Vector3){ 0.0f, 0.0f, 0.0f }, (Vector4){ 1.0f, 1.0f, 1.0f, 1.0f }, (Vector3){ 1.0f, 1.0f, 1.0f } }

void initShaders();

// Send all lights to the shader
void sendLightsToShader(Shader &shader, const std::vector<Light> &lights);
