//
// Created by micha on 8/20/2020.
//

#include <assimp/Importer.hpp>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "logging.h"
#include "assets.h"

inline aiMesh* openMesh(AAssetManager* assetManager, const char* asset, const char* ext) {
    AAsset* mesh = AAssetManager_open(assetManager, asset, AASSET_MODE_BUFFER);
    ASSERT(mesh, "could not open asset %s", asset);
    long length = AAsset_getLength(mesh);
    const void* buffer = AAsset_getBuffer(mesh);

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFileFromMemory(buffer, length,
                                                       aiProcess_CalcTangentSpace | aiProcess_Triangulate |
                                                       aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices | aiProcess_FlipUVs, ext);

    ASSERT(scene, "assimp could not import mesh file %s", asset);
    ASSERT(scene->mNumMeshes, "assimp could not find mesh data in file %s", asset);
    AAsset_close(mesh);

    return scene->mMeshes[0];
}
inline void loadVertices(aiMesh* mesh, float* buffer) {
    for (int i = 0; i < mesh->mNumVertices; i++) {
        const aiVector3D& pos = mesh->mVertices[i];
        const aiVector3D& textureCoord = (mesh->HasTextureCoords(0))
                                         ? mesh->mTextureCoords[0][i] : aiVector3D();
        const aiVector3D& normal = (mesh->HasNormals())
                                   ? mesh->mNormals[i] : aiVector3D();
        const aiVector3D& tangent = (mesh->HasTangentsAndBitangents())
                                    ? mesh->mTangents[i] : aiVector3D();

        const int index = i * VERTEX_SIZE;
        buffer[index + 0] = pos.x;
        buffer[index + 1] = pos.y;
        buffer[index + 2] = pos.z;
        buffer[index + 3] = textureCoord.x;
        buffer[index + 4] = textureCoord.y;
        buffer[index + 5] = normal.x;
        buffer[index + 6] = normal.y;
        buffer[index + 7] = normal.z;
        buffer[index + 8] = tangent.x;
        buffer[index + 9] = tangent.y;
        buffer[index + 10] = tangent.z;
    }
}
inline void loadIndices(aiMesh* mesh, uint* indices) {
    for (int i = 0; i < mesh->mNumFaces; i++) {
        const aiFace& face = mesh->mFaces[i];

        const int index = i * 3;
        indices[index + 0] = face.mIndices[0];
        indices[index + 1] = face.mIndices[1];
        indices[index + 2] = face.mIndices[2];
    }
}
inline uint createVAO(uint* indices, uint indicesSize, float* vertices, uint verticesSize) {
    uint error = glGetError();
    if (error) LOGW("warning: found GL error 0x%04x", error);

    uint buffers[3];
    glGenVertexArrays(1, buffers);
    glGenBuffers(2, buffers + 1);
    error = glGetError();
    ASSERT(error == GL_NO_ERROR, "GL error: could not generate buffer objects! error 0x%04x", error);

    glBindVertexArray(buffers[0]);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, indices, GL_STATIC_DRAW);
    error = glGetError();
    ASSERT(error == GL_NO_ERROR, "GL error: could not set ibo data! error 0x%04x", error);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ARRAY_BUFFER, verticesSize, vertices, GL_STATIC_DRAW);
    error = glGetError();
    ASSERT(error == GL_NO_ERROR, "GL error: could not set vbo data! error 0x%04x", error);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 44, nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, false, 44, ((void*) 12));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, false, 44, ((void*) 20));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, false, 44, ((void*) 32));

    error = glGetError();
    ASSERT(error == GL_NO_ERROR, "GL error: could not set vertex attrib pointers! error 0x%04x", error);

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return buffers[0];
}

void openAndAddShader(AAssetManager* assetManager, int program, const char* fileName, int type) {
    AAsset* asset = AAssetManager_open(assetManager, fileName, AASSET_MODE_BUFFER);
    const char* source = (const char*) AAsset_getBuffer(asset);
    int length = AAsset_getLength(asset);

    int shader = glCreateShader(type);
    ASSERT(shader, "GLES could not create shader");

    glShaderSource(shader, 1, &source, &length);
    glCompileShader(shader);

    int logLength;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
    char* log = new char[logLength];
    glGetShaderInfoLog(shader, logLength, nullptr, log);
    LOGI("%s", log);
    delete[] log;
    AAsset_close(asset);

    glAttachShader(program, shader);
}

void AssetLoader::load() {
    for (; loaded < assets.size(); loaded++) {
        LoadTask& task = assets[loaded];
        switch (task.type) {
            case MISERY_ASSET_MESH: loadMesh(task); break;
            case MISERY_ASSET_SHADER: loadShader(task); break;
            case MISERY_ASSET_TEXTURE: loadTexture(task); break;
        }
    }
}

void AssetLoader::loadMesh(AssetLoader::LoadTask &task) const {
    // find file extension for assimp
    std::string ext(*task.asset);
    ext = ext.substr(ext.find_last_of('.') + 1);
    // open asset with assetManager & assimp
    aiMesh* mesh = openMesh(assetManager, task.asset->c_str(), ext.c_str());
    uint indices[mesh->mNumFaces * 3];
    float vertices[mesh->mNumVertices * VERTEX_SIZE];

    // move mesh data from aiMesh to buffers
    loadIndices(mesh, indices);
    loadVertices(mesh, vertices);
    uint vao = createVAO(indices, sizeof(indices), vertices, sizeof(vertices));

    task.id_promise.set_value(new uint[] { vao, mesh->mNumFaces * 3 });
    delete task.asset;
}

void AssetLoader::loadShader(AssetLoader::LoadTask &task) const {
    GLuint id = glCreateProgram();
    ASSERT(id, "GLES could not create shader program");

    openAndAddShader(assetManager, id, task.asset[0].c_str(), GL_VERTEX_SHADER);
    openAndAddShader(assetManager, id, task.asset[1].c_str(), GL_FRAGMENT_SHADER);

    glLinkProgram(id);
    glValidateProgram(id);

    int logLength;
    glGetProgramiv(id, GL_INFO_LOG_LENGTH, &logLength);
    char* log = new char[logLength];
    glGetProgramInfoLog(id, logLength, nullptr, log);
    LOGI("%s", log);
    delete[] log;

    task.id_promise.set_value(new uint(id));
    delete task.asset;
}

void AssetLoader::loadTexture(AssetLoader::LoadTask &task) const {
    GLuint id;
    glGenTextures(1, &id);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    AndroidBitmapInfo& info = task.texture->first;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, info.width, info.height,
            0, GL_RGBA, GL_UNSIGNED_BYTE, task.texture->second);

    glBindTexture(GL_TEXTURE_2D, 0);
    task.id_promise.set_value(new uint(id));

    std::free(task.texture->second);
    delete task.texture;
}

jobject AssetLoader::openJBitmap(JNIEnv *env, const char *asset) {
    // open asset
    AAsset* texture = AAssetManager_open(assetManager, asset, AASSET_MODE_BUFFER);
    const void* buffer = AAsset_getBuffer(texture);
    int length = AAsset_getLength(texture);
    // move data to a java byte array in order to pass it to BitmapFactory
    jbyteArray byteArray = env->NewByteArray(length);
    env->SetByteArrayRegion(byteArray, 0, length, (jbyte*) buffer);
    AAsset_close(texture);
    // get the BitmapFactory.decodeByteArray() static method
    if (decodeByteArray == nullptr) {
        bitmapFactory = (jclass) env->NewGlobalRef(env->FindClass("android/graphics/BitmapFactory"));
        decodeByteArray = env->GetStaticMethodID(bitmapFactory, "decodeByteArray","([BII)Landroid/graphics/Bitmap;");
    }
    jobject bitmap = env->CallStaticObjectMethod(bitmapFactory, decodeByteArray, byteArray, 0, length);
    JNI_CATCH_EXCEPTION("java exception occurred when trying to load texture asset %s", asset);
    return bitmap;
}

void AssetLoader::prepareTextureTask(JNIEnv *env, uint index) {
    jobject bitmap = openJBitmap(env, assets[index].asset->c_str());
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
    assets[index].texture = new std::pair<AndroidBitmapInfo, void*> { info, buffer };
    // only the RGBA_8888 android format is supported by OpenGL
    ASSERT(assets[index].texture->first.format == ANDROID_BITMAP_FORMAT_RGBA_8888,
           "unsupported texture format!");
}

AssetID AssetLoader::addMesh(const char *asset) {
    // check if this mesh asset was already loaded
    for (LoadTask& t : assets)
        if (asset == t.asset[0]) return AssetID(t.id);
    LOGI("added mesh asset %s to queue", asset);
    assets.push_back(LoadTask { .asset = new std::string(asset), .type = MISERY_ASSET_MESH });
    return assets.back().id;
}

AssetID AssetLoader::addShader(const char *vertex, const char *fragment) {
    // check if a matching shader was already loaded
    for (LoadTask& t : assets)
        if (vertex == t.asset[0] && fragment == t.asset[1]) return AssetID(t.id);
    LOGI("added shader program assets (vertex: %s, fragment: %s) to queue", vertex, fragment);
    std::string* shaders = new std::string[] { std::string(vertex), std::string(fragment) };
    assets.push_back(LoadTask { .asset = shaders , .type = MISERY_ASSET_SHADER });
    return assets.back().id;
}

AssetID AssetLoader::addTexture(JNIEnv *env, const char *asset) {
    assets.push_back(LoadTask { .asset = new std::string(asset) , .type = MISERY_ASSET_TEXTURE });
    if (assetManager == nullptr) pendingTextures.push_back(assets.size() - 1);
    else prepareTextureTask(env, assets.size() - 1);
    return assets.back().id;
}

void AssetLoader::setAssetManager(JNIEnv *env, AAssetManager *aassetManager) {
    this->assetManager = aassetManager;
    for (uint i : pendingTextures) prepareTextureTask(env, i);
    pendingTextures.clear();
}
