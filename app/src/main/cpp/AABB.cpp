//
// Created by micha on 9/2/2020.
//

#include "AABB.h"

AABB AABB::transform(matrix4f &matrix) {
    vector3f tmin = matrix * vector3f(min.x, min.y, min.z), tmax(tmin);
    vector3f points[] = {
            matrix * vector3f(max.x, min.y, max.z),
            matrix * vector3f(min.x, max.y, min.z),
            matrix * vector3f(min.x, min.y, max.z),
            matrix * vector3f(min.x, max.y, max.z),
            matrix * vector3f(max.x, min.y, min.z),
            matrix * vector3f(max.x, max.y, min.z),
            matrix * vector3f(max.x, max.y, max.z)
    };
    for (auto& p : points) {
        tmin = vector3f(std::min(tmin.x, p.x), std::min(tmin.y, p.y), std::min(tmin.z, p.z));
        tmax = vector3f(std::max(tmax.x, p.x), std::max(tmax.y, p.y), std::max(tmax.z, p.z));
    }

    return { tmin, tmax };
}
