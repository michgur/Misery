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
    /* textures that have been added before assetManager was set */
    std::vector<uint> pendingTextures;

    void loadMesh(LoadTask& task) const;
    void loadShader(LoadTask& task) const;
    void loadTexture(LoadTask& task) const;

    jclass bitmapFactory = nullptr;
    jmethodID decodeByteArray = nullptr;
    /* open an asset and use its data to create a java bitmap via BitmapFactory */
    jobject openJBitmap(JNIEnv* env, const char* asset);
    /* use JNI to extract the data from the asset, so we'll be able to pass it to OpenGL later */
    void prepareTextureTask(JNIEnv* env, uint index);
public:
    void setAssetManager(JNIEnv* env, AAssetManager* aassetManager);

    inline bool empty() const { return loaded == assets.size(); }
    void load();

    AssetID addMesh(const char* asset);
    AssetID addShader(const char* vertex, const char* fragment);
    /** since native image decoding is only available since api level >= 30,
     *  we need to rely on java methods to get a bitmap from the asset */
    AssetID addTexture(JNIEnv* env, const char* asset);
};


#endif //MISERY_ASSETS_H
