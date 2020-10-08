//
// Created by micha on 8/20/2020.
//

#ifndef MISERY_ASSETS_H
#define MISERY_ASSETS_H

#include <GLES3/gl3.h>
#include <jni.h>
#include <future>
#include <android/asset_manager.h>
#include <android/bitmap.h>
#include <vector>

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
        AssetID id = id_promise.get_future().share();
        uint8_t type;
    };

    AAssetManager* assetManager;
    /* all of the assets that have been queued to be loaded / that have been loaded */
    std::vector<LoadTask> assets = std::vector<LoadTask>();
    /* the amount of assets that have been loaded */
    uint loaded = 0;

    void loadMesh(LoadTask& task) const;
    void loadShader(LoadTask& task) const;
    void loadTexture(LoadTask& task) const;
public:
    inline void setAssetManager(AAssetManager* aassetManager) { this->assetManager = aassetManager; }

    inline bool empty() const { return loaded == assets.size(); }
    void load();

    inline AssetID loadMesh(const char* asset) {
        // check if this mesh asset was already loaded
        for (LoadTask& t : assets)
            if (asset == t.asset[0]) return AssetID(t.id);
        LOGI("added mesh asset %s to queue", asset);
        assets.push_back(LoadTask { .asset = new std::string(asset), .type = MISERY_ASSET_MESH });
        return assets.back().id;
    }
    inline AssetID loadShader(const char* vertex, const char* fragment) {
        // check if a matching shader was already loaded
        for (LoadTask& t : assets)
            if (vertex == t.asset[0] && fragment == t.asset[1]) return AssetID(t.id);
        LOGI("added shader program assets (vertex: %s, fragment: %s) to queue", vertex, fragment);
        std::string* shaders = new std::string[] { std::string(vertex), std::string(fragment) };
        assets.push_back(LoadTask { .asset = shaders , .type = MISERY_ASSET_SHADER });
        return assets.back().id;
    }

    /** since native image decoding is only available since api level >= 30,
     *  we need to rely on java methods to get a bitmap from the asset */
    inline AssetID loadTexture(JNIEnv* env, jobject bitmap) {
        assets.push_back(LoadTask { .type = MISERY_ASSET_TEXTURE });
        // since we need a JNI env in order to get the bitmap info, we copy it here &
        // store it in the LoadTask, which will later pass it to OpenGL in the EGL thread
        AndroidBitmapInfo info;
        AndroidBitmap_getInfo(env, bitmap, &info);
        int size = sizeof(uint32_t) * info.width * info.height;
        void *src, *buffer = std::malloc(size);
        AndroidBitmap_lockPixels(env, bitmap, &src);
        std::memcpy(buffer, src, size);
        // release the bitmap (data is copied)
        AndroidBitmap_unlockPixels(env, bitmap);
        assets.back().texture = new std::pair<AndroidBitmapInfo, void*> { info, buffer };
        // only the RGBA_8888 android format is supported by OpenGL
        ASSERT(assets.back().texture->first.format == ANDROID_BITMAP_FORMAT_RGBA_8888,
                "unsupported texture format!");
        return assets.back().id;
    }
};


#endif //MISERY_ASSETS_H
