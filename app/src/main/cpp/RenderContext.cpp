//
// Created by micha on 8/28/2020.
//

#include "RenderContext.h"
#include <GLES3/gl3.h>
#include "assets.h"


namespace Misery { RenderContext renderContext = RenderContext(); }

void RenderContext::setViewSize(uint w, uint h) {
    width = w;
    height = h;

    projection = Misery::createProjectionMatrix(std::acos(0), width, height, 0.1, 1000);
}
