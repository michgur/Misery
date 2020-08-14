//
// Created by micha on 8/13/2020.
//

#include <jni.h>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/port/AndroidJNI/AndroidJNIIOSystem.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>
#include <GLES3/gl3.h>

#define ASSERT(condition, ...) if (!condition) __android_log_assert(#condition, "misery-assets", __VA_ARGS__)

struct Mesh { const GLuint vao, size; };

extern "C"
JNIEXPORT jlong JNICALL
Java_com_klmn_misery_Mesh_00024Companion_loadMesh(JNIEnv *env, jobject thiz, jobject jassetManager,
                                                  jstring file, jstring ext) {
    const char* fileString = env->GetStringUTFChars(file, nullptr);
    const char* extString = env->GetStringUTFChars(ext, nullptr);

    AAssetManager* assetManager = AAssetManager_fromJava(env, jassetManager);
    AAsset* mesh = AAssetManager_open(assetManager, fileString, AASSET_MODE_BUFFER);
    ASSERT(mesh, "could not open asset %s", fileString);
    long length = AAsset_getLength(mesh);
    const void* buffer = AAsset_getBuffer(mesh);

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFileFromMemory(buffer, length,
                                                       aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_GenSmoothNormals |
                                                       aiProcess_JoinIdenticalVertices | aiProcess_FlipUVs, extString);

    ASSERT(scene, "assimp could not import mesh file %s", fileString);
    ASSERT(scene->mNumMeshes, "assimp could not find mesh data in file %s", fileString);

    const aiMesh* meshData = scene->mMeshes[0];
    unsigned int indices[meshData->mNumFaces * 3];
    float vertices[meshData->mNumVertices * 11];

    for (int i = 0; i < meshData->mNumVertices; i++) {
        const aiVector3D& pos = meshData->mVertices[i];
        const aiVector3D& textureCoord = (meshData->HasTextureCoords(0))
                                         ? meshData->mTextureCoords[0][i] : aiVector3D();
        const aiVector3D& normal = (meshData->HasNormals())
                                   ? meshData->mNormals[i] : aiVector3D();
        const aiVector3D& tangent = (meshData->HasTangentsAndBitangents())
                                    ? meshData->mTangents[i] : aiVector3D();

        const int index = i * 11;
        vertices[index + 0] = pos.x;
        vertices[index + 1] = pos.y;
        vertices[index + 2] = pos.z;
        vertices[index + 3] = textureCoord.x;
        vertices[index + 4] = textureCoord.y;
        vertices[index + 5] = normal.x;
        vertices[index + 6] = normal.y;
        vertices[index + 7] = normal.z;
        vertices[index + 8] = tangent.x;
        vertices[index + 9] = tangent.y;
        vertices[index + 10] = tangent.z;
    }
    for (int i = 0; i < meshData->mNumFaces; i++) {
        const aiFace& face = meshData->mFaces[i];

        const int index = i * 3;
        indices[index + 0] = face.mIndices[0];
        indices[index + 1] = face.mIndices[1];
        indices[index + 2] = face.mIndices[2];
    }

    GLuint buffers[3];
    glGenVertexArrays(1, buffers);
    glGenBuffers(2, buffers + 1);

    glBindVertexArray(buffers[0]);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 44, nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, false, 44, ((void*) 12));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, false, 44, ((void*) 20));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, false, 44, ((void*) 32));

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    AAsset_close(mesh);
    env->ReleaseStringUTFChars(file, fileString);
    env->ReleaseStringUTFChars(ext, extString);

    Mesh* result = new Mesh {buffers[0], (GLuint) (meshData->mNumFaces * 3)};
    return (long) (result);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_klmn_misery_Mesh_00024Companion_drawMesh(JNIEnv *env, jobject thiz, jlong pointer) {
    glBindVertexArray(((Mesh*) pointer)->vao);
    glDrawElements(GL_TRIANGLES, ((Mesh*) pointer)->size, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}