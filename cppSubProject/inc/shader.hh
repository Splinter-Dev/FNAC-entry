#pragma once

#include <vector>
#include <raylib.h>
#include "light.hh"

#define DEFAULT_AMBIENT Vector4{ 0.3f, 0.3f, 0.3f, 1.0f }
#define DEFAULT_VIEW_POS Vector3{ 0.0f, 0.0f, 0.0f }

extern Shader toonShader;
extern int ambientLoc;
extern int viewPosLoc;

void initShaders();
void sendLightsToShader(Shader &shader, const std::vector<Light> &lights);
