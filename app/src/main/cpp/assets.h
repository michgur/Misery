//
// Created by micha on 8/20/2020.
//

#ifndef MISERY_ASSETS_H
#define MISERY_ASSETS_H

#include <GLES3/gl3.h>
#include <jni.h>
#include <android/asset_manager.h>

#define VERTEX_SIZE 11

namespace Misery {
    extern AAssetManager* assetManager;

    struct Mesh { const GLuint vao, size; };
    Mesh* loadMeshFromAsset(const char* asset, const char* ext);

    uint createShaderProgram(const char* vertex, const char* fragment);
}


#endif //MISERY_ASSETS_H
