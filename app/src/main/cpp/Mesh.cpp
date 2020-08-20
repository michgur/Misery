//
// Created by micha on 8/20/2020.
//

#include <assimp/Importer.hpp>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "logging.h"
#include "Mesh.h"

namespace Misery { AAssetManager* assetManager; }

Misery::Mesh* Misery::loadMeshFromAsset(const char* asset, const char* ext) {
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

    return new Misery::Mesh {buffers[0], (GLuint) (meshData->mNumFaces * 3)};
}
