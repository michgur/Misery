package com.klmn.misery

import android.content.res.AssetManager
import java.lang.System

object MiseryJNI
{
    init { System.loadLibrary("misery-native-lib") }

    external fun setNativeAssetManager(assetManager: AssetManager)
    external fun loadMesh(file: String, ext: String): Long
    external fun drawMesh(pointer: Long)
    external fun createProgram(vertexFile: String, fragmentFile: String): Int
    external fun createEntity(): Int
    external fun putComponent(entity: Int, type: String, value: Any)
    external fun getComponent(entity: Int, type: String): Any
}