package com.klmn.misery

import android.content.res.AssetManager
import java.lang.System.loadLibrary

class Mesh(assets: AssetManager, path: String)
{
    private companion object {
        init { loadLibrary("misery-native-lib") }

        external fun loadMesh(assetManager: AssetManager, file: String, ext: String): Long
        external fun drawMesh(pointer: Long)
    }

    private val pointer: Long = loadMesh(assets, path, path.split(".").last())
    fun draw() = drawMesh(pointer)
}
