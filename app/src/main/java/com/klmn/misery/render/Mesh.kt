package com.klmn.misery.render

import com.klmn.misery.jni.MiseryJNI

class Mesh(path: String)
{
    val pointer: Long = MiseryJNI.loadMesh(path)
    fun draw() = MiseryJNI.drawMesh(pointer)
}
