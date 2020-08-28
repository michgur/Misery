package com.klmn.misery

class Mesh(path: String)
{
    val pointer: Long = MiseryJNI.loadMesh(path, path.split(".").last())
    fun draw() = MiseryJNI.drawMesh(pointer)
}
