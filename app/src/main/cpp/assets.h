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
#include <android/bitmap.h>

#define VERTEX_SIZE 11

#define MISERY_ASSET_MESH 1
#define MISERY_ASSET_SHADER 2
#define MISERY_ASSET_TEXTURE 3

typedef std::shared_future<uint*> AssetID;

class AssetLoader {
    struct LoadTask {
        union {
            std::string* asset;
            std::pair<AndroidBitmapInfo, void*>* texture;
        };
        std::promise<uint*> id_promise;
        uint8_t type;
    };

    AAssetManager* assetManager;
    std::queue<LoadTask> tasks = std::queue<LoadTask>();
    std::queue<LoadTask> loaded = std::queue<LoadTask>();

    void loadMesh(LoadTask& task) const;
    void loadShader(LoadTask& task) const;
    void loadTexture(LoadTask& task) const;
public:
    inline void setAssetManager(AAssetManager* aassetManager) { this->assetManager = aassetManager; }

    inline bool empty() const { return tasks.empty(); }
    void load();

    inline AssetID loadMesh(const char* asset) {
        tasks.push(LoadTask { new std::string(asset), std::promise<uint*>(), MISERY_ASSET_MESH });
        return AssetID(tasks.back().id_promise.get_future());
    }
    inline AssetID loadShader(const char* vertex, const char* fragment) {
        std::string* assets = new std::string[] { std::string(vertex), std::string(fragment) };
        tasks.push(LoadTask { assets , std::promise<uint*>(), MISERY_ASSET_SHADER });
        return AssetID(tasks.back().id_promise.get_future());
    }

    /** since native image decoding is only available since api level >= 30,
     *  we need to rely on java methods to get a bitmap from the asset */
    inline AssetID loadTexture(JNIEnv* env, jobject bitmap) {
        tasks.push(LoadTask { .type = MISERY_ASSET_TEXTURE });
        // since we need a JNI env in order to get the bitmap info, we copy it here &
        // store it in the LoadTask, which will later pass it to OpenGL in the EGL thread
        AndroidBitmapInfo info;
        AndroidBitmap_getInfo(env, bitmap, &info);
        int size = sizeof(uint32_t) * info.width * info.height;
        void *src, *buffer = std::malloc(size);
        AndroidBitmap_lockPixels(env, bitmap, &src);
        std::memcpy(src, buffer, size);
        // release the bitmap (data is copied)
        AndroidBitmap_unlockPixels(env, bitmap);
        tasks.back().texture = new std::pair<AndroidBitmapInfo, void*> { info, buffer };
        // only the RGBA_8888 android format is supported by OpenGL
        ASSERT(tasks.back().texture->first.format == ANDROID_BITMAP_FORMAT_RGBA_8888,
                "unsupported texture format!");
        return AssetID(tasks.back().id_promise.get_future());
    }
};


#endif //MISERY_ASSETS_H
