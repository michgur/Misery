package com.klmn.misery.render

import com.klmn.misery.jni.MiseryJNI

class Mesh(path: String)
{
    val pointer: Long = MiseryJNI.loadMesh(path, path.split(".").last())
    fun draw() = MiseryJNI.drawMesh(pointer)
}
