package com.klmn.misery

class Mesh(path: String)
{
    private companion object {
        external fun loadMesh(file: String, ext: String): Long
        external fun drawMesh(pointer: Long)
    }

    private val pointer: Long = loadMesh(path, path.split(".").last())
    fun draw() = drawMesh(pointer)
}
