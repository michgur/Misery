//
// Created by micha on 8/17/2020.
//

#ifndef MISERY_TRANSFORM_H
#define MISERY_TRANSFORM_H

#include <assimp/vector3.h>
#include <assimp/quaternion.h>
#include <assimp/matrix4x4.h>
#include <jni.h>
#include "ECS.h"

#define TRANSFORM_SIZE 10

typedef aiVector3D vector3f;
typedef aiQuaternion quaternion;
typedef aiMatrix4x4 matrix4f;

struct Transform {
    union {
        struct {
            vector3f translation = vector3f(0);
            quaternion rotation = quaternion();
            vector3f scale = vector3f(1);
        };
        float data[TRANSFORM_SIZE];
    };

    Transform() {}

    inline matrix4f toMatrix();
};


#endif //MISERY_TRANSFORM_H
