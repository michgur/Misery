//
// Created by micha on 9/2/2020.
//

#ifndef MISERY_AABB_H
#define MISERY_AABB_H

#include "Transform.h"
#include <assimp/vector3.inl>

struct AABB {
    union {
        struct {
            vector3f min = vector3f(0);
            vector3f max = vector3f(0);
        };
        float data[6];
    };

    AABB() {}
    AABB(vector3f min, vector3f max) : min(min), max(max) {}

    inline vector3f getCenter() { return (max + min) / 2.0f; }
    inline vector3f getExtents() { return (max - min) / 2.0f; }
    inline bool intersects(AABB& other) {
        return !(min.x > other.max.x || max.x < other.min.x || min.y > other.max.y ||
                 max.y < other.min.y || min.z > other.max.z || max.z < other.min.z);
    }

    AABB transform(matrix4f& matrix);
};

#endif //MISERY_AABB_H
