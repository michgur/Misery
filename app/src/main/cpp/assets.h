//
// Created by micha on 8/20/2020.
//

#ifndef MISERY_ASSETS_H
#define MISERY_ASSETS_H

#include <GLES3/gl3.h>
#include <jni.h>
#include <queue>
#include <future>
#include <android/asset_manager.h>

#define VERTEX_SIZE 11

namespace Misery {
    extern AAssetManager* assetManager;

    struct Mesh { const GLuint vao, size; };
    Mesh* loadMeshFromAsset(const char* asset, const char* ext);

    uint createShaderProgram(const char* vertex, const char* fragment);
}

//class AssetLoader {
//    struct LoadTask {
//        const char** asset;
//        std::promise<uint> id_promise;
//        uint8_t type; // 1 = mesh, 2 = shader, 3 = texture
//    };
//
//    AAssetManager* assetManager;
//    std::queue<LoadTask> tasks;
//
//    void loadMesh(LoadTask& task);
//    void loadShader(LoadTask& task);
//    void loadTexture(LoadTask& task);
//public:
//    inline bool empty() { return tasks.empty(); }
//    void load();
//};


#endif //MISERY_ASSETS_H
