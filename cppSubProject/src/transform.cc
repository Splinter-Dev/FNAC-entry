#include <raylib.h>
#include <raymath.h>
#include <cfloat>
#include "transform.hpp"

/* Transforms a bounding box with a matrix */
BoundingBox TransformBoundingBox(const BoundingBox &bb, const Matrix &m) {

    // Extract all 8 corners of the box
    Vector3 corners[8] = {
        {bb.min.x, bb.min.y, bb.min.z},
        {bb.min.x, bb.min.y, bb.max.z},
        {bb.min.x, bb.max.y, bb.min.z},
        {bb.min.x, bb.max.y, bb.max.z},
        {bb.max.x, bb.min.y, bb.min.z},
        {bb.max.x, bb.min.y, bb.max.z},
        {bb.max.x, bb.max.y, bb.min.z},
        {bb.max.x, bb.max.y, bb.max.z}
    };

    BoundingBox newBB;
    newBB.min = { FLT_MAX, FLT_MAX, FLT_MAX };
    newBB.max = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

    for (int i = 0; i < 8; i++) {
        Vector3 p = Vector3Transform(corners[i], m);

        newBB.min.x = fminf(newBB.min.x, p.x);
        newBB.min.y = fminf(newBB.min.y, p.y);
        newBB.min.z = fminf(newBB.min.z, p.z);

        newBB.max.x = fmaxf(newBB.max.x, p.x);
        newBB.max.y = fmaxf(newBB.max.y, p.y);
        newBB.max.z = fmaxf(newBB.max.z, p.z);
    }

    return newBB;
}

/* For some reason DrawModel doesn't receive a matrix, so we have to do it manually */
void DrawModelTransformed(Model model, Matrix transform) {
    rlPushMatrix();
    rlMultMatrixf(MatrixToFloat(transform)); // Apply the matrix
    DrawModel(model, {0,0,0}, 1.0f, WHITE);
    rlPopMatrix();
}
