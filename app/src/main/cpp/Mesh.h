//
// Created by micha on 8/20/2020.
//

#ifndef MISERY_MESH_H
#define MISERY_MESH_H

#include <GLES3/gl3.h>
#include <jni.h>
#include <android/asset_manager.h>

namespace Misery {
    extern AAssetManager* assetManager;

    struct Mesh { const GLuint vao, size; };
    Mesh* loadMeshFromAsset(const char* asset, const char* ext);
}


#endif //MISERY_MESH_H
