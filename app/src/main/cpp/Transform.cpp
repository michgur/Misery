//
// Created by micha on 8/17/2020.
//

#include "Transform.h"
#include <assimp/matrix4x4.inl>

matrix4f Transform::toMatrix() {
    matrix4f m = matrix4f();

    quaternion r2 = quaternion(rotation.w * 2, rotation.x * 2, rotation.y * 2, rotation.z * 2);
    float xx2 = rotation.x * r2.x;
    float yy2 = rotation.y * r2.y;
    float zz2 = rotation.z * r2.z;
    float xy2 = rotation.x * r2.y;
    float yz2 = rotation.y * r2.z;
    float xz2 = rotation.x * r2.z;
    float xw2 = rotation.w * r2.x;
    float yw2 = rotation.w * r2.y;
    float zw2 = rotation.w * r2.z;

    m.a1 = (1 - (yy2 + zz2)) * scale.x;
    m.a2 = (xy2 - zw2) * scale.y;
    m.a3 = (xz2 + yw2) * scale.z;
    m.a4 = translation.x;
    m.b1 = (xy2 + zw2) * scale.x;
    m.b2 = (1 - (xx2 + zz2)) * scale.y;
    m.b3 = (yz2 - xw2) * scale.z;
    m.b4 = translation.y;
    m.c1 = (xz2 - yw2) * scale.x;
    m.c2 = (yz2 + xw2) * scale.y;
    m.c3 = (1 - (xx2 + yy2)) * scale.z;
    m.c4 = translation.z;

    return m;
}

matrix4f Misery::createProjectionMatrix(float fov, float width, float height, float near, float far) {
    matrix4f mat4f;

    float ar = width / height;
    float tan = std::tan(fov / 2);
    float range = near - far;

    mat4f.a1 = -1 / (tan * ar);
    mat4f.b2 = 1 / tan;
    mat4f.c3 = (near + far) / range;
    mat4f.c4 = 2 * far * near / range;
    mat4f.d3 = -1;
    mat4f.d4 = 0;

    return mat4f;
}