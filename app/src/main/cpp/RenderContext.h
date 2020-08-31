//
// Created by micha on 8/28/2020.
//

#ifndef MISERY_RENDERCONTEXT_H
#define MISERY_RENDERCONTEXT_H

#include "Transform.h"
#include <assimp/matrix4x4.inl>
#include "ECS.h"

struct RenderContext {
    uint width = 0, height = 0;
    matrix4f projection;
    Transform camera;

    void (*render)(uint, float);
    void setViewSize(uint w, uint h);
};

namespace Misery { extern RenderContext renderContext; }

#endif //MISERY_RENDERCONTEXT_H
