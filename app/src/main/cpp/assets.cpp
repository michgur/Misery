//
// Created by micha on 8/20/2020.
//

#include <assimp/Importer.hpp>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "logging.h"
#include "assets.h"

namespace Misery { AAssetManager* assetManager; }

inline aiMesh* openMesh(const char* asset, const char* ext) {
    AAsset* mesh = AAssetManager_open(Misery::assetManager, asset, AASSET_MODE_BUFFER);
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
    uint buffers[3];
    glGenVertexArrays(1, buffers);
    glGenBuffers(2, buffers + 1);

    glBindVertexArray(buffers[0]);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ARRAY_BUFFER, verticesSize, vertices, GL_STATIC_DRAW);

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

    return buffers[0];
}
Misery::Mesh* Misery::loadMeshFromAsset(const char* asset, const char* ext) {
    aiMesh* mesh = openMesh(asset, ext);
    uint indices[mesh->mNumFaces * 3];
    float vertices[mesh->mNumVertices * VERTEX_SIZE];

    loadIndices(mesh, indices);
    loadVertices(mesh, vertices);
    uint vao = createVAO(indices, sizeof(indices), vertices, sizeof(vertices));

    return new Misery::Mesh { vao, (GLuint) (mesh->mNumFaces * 3) };
}

void openAndAddShader(int program, const char* fileName, int type) {
    AAsset* asset = AAssetManager_open(Misery::assetManager, fileName, AASSET_MODE_BUFFER);
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
uint Misery::createShaderProgram(const char* vertex, const char* fragment) {
    GLuint id = glCreateProgram();
    ASSERT(id, "GLES could not create shader program");
    glUseProgram(id);

    openAndAddShader(id, vertex, GL_VERTEX_SHADER);
    openAndAddShader(id, fragment, GL_FRAGMENT_SHADER);

    glLinkProgram(id);
    glValidateProgram(id);

    int logLength;
    glGetProgramiv(id, GL_INFO_LOG_LENGTH, &logLength);
    char* log = new char[logLength];
    glGetProgramInfoLog(id, logLength, nullptr, log);
    LOGI("%s", log);
    delete[] log;

    glUseProgram(0);
    return id;
}
