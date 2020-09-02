//
// Created by micha on 9/2/2020.
//

#ifndef MISERY_PHYSICS_H
#define MISERY_PHYSICS_H

#include "Transform.h"
#include <assimp/vector3.inl>


struct Motion {
    union {
        struct {
            vector3f velocity = vector3f(0);
            vector3f acceleration = vector3f(0);
        };
        float data[6];
    };

    Motion() {}
    Motion(vector3f velocity, vector3f acceleration) : velocity(velocity), acceleration(acceleration) {}
};

vector3f verlet(vector3f pos, Motion& motion, float delta);
vector3f forestRuth(vector3f& pos, Motion& motion, float delta);

void motionSystem(uint entity, float delta);


#endif //MISERY_PHYSICS_H
