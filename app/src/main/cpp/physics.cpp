//
// Created by micha on 9/2/2020.
//

#include "physics.h"
#include <pthread.h>

float coefficient = 1.0f / (2.0f - std::pow(2.0f, 1.0f / 3.0f));
float complement = 1.0f - 2.0f * coefficient;

vector3f verlet(vector3f pos, Motion &motion, float delta) {
    vector3f res = pos;
    float halfDelta = delta / 2.0f;
    res += motion.velocity * halfDelta;
    motion.velocity += motion.acceleration * delta;
    return res + motion.velocity * halfDelta;
}

vector3f forestRuth(vector3f &pos, Motion &motion, float delta) {
    vector3f res = verlet(pos, motion, delta * coefficient);
    res = verlet(res, motion, delta * complement);
    return verlet(res, motion, delta * coefficient);
}

void motionSystem(uint entity, float delta) {
    auto* transform = Misery::ecs.getComponent<Transform>(entity, "transform");
    auto* motion = Misery::ecs.getComponent<Motion>(entity, "motion");

    transform->translation = forestRuth(transform->translation, *motion, delta);
    motion->velocity *= .95f;
}
