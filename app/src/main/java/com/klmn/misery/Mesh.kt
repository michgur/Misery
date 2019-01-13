package com.klmn.misery

import android.content.res.AssetManager
import android.opengl.GLES30.*
import assimp.AiPostProcessStep
import assimp.AiVector3D
import assimp.Importer
import assimp.or
import java.nio.ByteBuffer
import java.nio.ByteOrder

/**
 * ಠ^ಠ.
 * Created by Michael on 12/15/2018.
 */
class Mesh(assets: AssetManager, path: String)
{
    private var vao: Int = 0
    private var size: Int = 0

    init {
        val (vertices, indices) = load(assets, path)
        size = indices.size

        val intArray = IntArray(3)
        glGenVertexArrays(1, intArray, 2)
        glGenBuffers(2, intArray, 0)
        vao = intArray[2]
        val (vbo, ibo) = intArray

        glBindVertexArray(vao)

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo)
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size * 4, createIntBuffer(indices), GL_STATIC_DRAW)

        glBindBuffer(GL_ARRAY_BUFFER, vbo)
        glBufferData(GL_ARRAY_BUFFER, vertices.size * 4, createFloatBuffer(vertices), GL_STATIC_DRAW)

        glEnableVertexAttribArray(0)
        glVertexAttribPointer(0, 3, GL_FLOAT, false, 44, 0)
        glEnableVertexAttribArray(1)
        glVertexAttribPointer(1, 2, GL_FLOAT, false, 44, 12)
        glEnableVertexAttribArray(2)
        glVertexAttribPointer(2, 3, GL_FLOAT, false, 44, 20)
        glEnableVertexAttribArray(3)
        glVertexAttribPointer(3, 3, GL_FLOAT, false, 44, 32)

        glBindVertexArray(0)
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0)
        glBindBuffer(GL_ARRAY_BUFFER, 0)
    }

    fun draw() {
        glBindVertexArray(vao)
        glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, 0)
        glBindVertexArray(0)
    }
}

private fun createFloatBuffer(data: FloatArray) =
        ByteBuffer.allocateDirect(data.size * 4)
                .order(ByteOrder.nativeOrder())
                .asFloatBuffer()
                .put(data)
                .position(0)
private fun createIntBuffer(data: IntArray) =
        ByteBuffer.allocateDirect(data.size * 4)
                .order(ByteOrder.nativeOrder())
                .asIntBuffer()
                .put(data)
                .position(0)

private val flags = AiPostProcessStep.Triangulate or
        AiPostProcessStep.GenSmoothNormals or
        AiPostProcessStep.FlipUVs or
        AiPostProcessStep.CalcTangentSpace

/* Uses https://github.com/kotlin-graphics/assimp to load 3d mesh files */
private fun load(assets: AssetManager, path: String) : Pair<FloatArray, IntArray> {
    val scene = Importer().readFileFromMemory(
            ByteBuffer.wrap(assets.open(path).readBytes()),
            flags,
            path.split(".").last()
    ) ?: throw RuntimeException()
    assert(scene.numMeshes > 0)

    val mesh = scene.meshes[0]
    val vertices = FloatArray(mesh.numVertices * 11)
    for (i in 0 until mesh.numVertices) {
        val pos = mesh.vertices[i]
        val textureCoord = if (mesh.hasTextureCoords(0)) mesh.textureCoords[0][i] else floatArrayOf(0f, 0f)
        val normal = mesh.normals[i]
        val tangent = if (mesh.hasTangentsAndBitangents) mesh.tangents[i] else AiVector3D(0f, 0f, 0f)

        val index = i * 11
        vertices[index + 0] = pos.x
        vertices[index + 1] = pos.y
        vertices[index + 2] = pos.z
        vertices[index + 3] = textureCoord[0]
        vertices[index + 4] = textureCoord[1]
        vertices[index + 5] = normal.x
        vertices[index + 6] = normal.y
        vertices[index + 7] = normal.z
        vertices[index + 8] = tangent.x
        vertices[index + 9] = tangent.y
        vertices[index + 10] = tangent.z
    }
    val indices = IntArray(mesh.numFaces * 3)
    for (i in 0 until mesh.numFaces) {
        val face = mesh.faces[i]

        val index = i * 3
        indices[index + 0] = face[0]
        indices[index + 1] = face[1]
        indices[index + 2] = face[2]
    }

    return Pair(vertices, indices)
}
