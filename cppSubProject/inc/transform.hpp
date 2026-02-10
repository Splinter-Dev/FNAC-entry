#pragma once

#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

/* Transforms a bounding box with a matrix */
BoundingBox TransformBoundingBox(const BoundingBox &bb, const Matrix &m);

/* For some reason DrawModel doesn't receive a matrix, so we have to do it manually */
void DrawModelTransformed(Model model, Matrix transform);
