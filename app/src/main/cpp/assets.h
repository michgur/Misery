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
//    Mesh* loadMeshFromAsset(const char* asset, const char* ext);

//    uint createShaderProgram(const char* vertex, const char* fragment);
}

#define MISERY_ASSET_MESH 1
#define MISERY_ASSET_SHADER 2
#define MISERY_ASSET_TEXTURE 3

class AssetLoader {
    struct LoadTask {
        std::string* asset;
        std::promise<uint> id_promise;
        uint8_t type;
    };

    AAssetManager* assetManager;
    std::queue<LoadTask> tasks;

    void loadMesh(LoadTask& task);
    void loadShader(LoadTask& task);
    void loadTexture(LoadTask& task);
public:
    inline void setAssetManager(AAssetManager* assetManager) { this->assetManager = assetManager; }

    inline bool empty() { return tasks.empty(); }
    void load();

    inline std::promise<uint>& loadMesh(const char* asset) {
        tasks.push(LoadTask { new std::string(asset), std::promise<uint>(), 1 });
        return tasks.back().id_promise;
    }
    inline std::promise<uint>& loadShader(const char* vertex, const char* fragment) {
        std::string* assets = new std::string[] { std::string(vertex), std::string(fragment) };
        tasks.push(LoadTask { assets , std::promise<uint>(), 2 });
        return tasks.back().id_promise;
    }
    inline std::promise<uint>& loadTexture(const char* asset) {
        tasks.push(LoadTask { new std::string(asset), std::promise<uint>(), 3 });
        return tasks.back().id_promise;
    }
};


#endif //MISERY_ASSETS_H
