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
    external fun createEntity(wrapper: Entity): Int
    external fun putComponent(entity: Int, type: String, value: Any)
    external fun getComponent(entity: Int, type: String): Any
    external fun addSystem(types: Array<String>, apply: (Entity, Float) -> Unit)
    external fun updateECS(delta: Float)
    external fun clearECS()
    external fun getFloat(pointer: Long, offset: Int): Float
    external fun setFloat(pointer: Long, f: Float, offset: Int)
    external fun getFloats(pointer: Long, count: Int, offset: Int): FloatArray
    external fun setFloats(pointer: Long, f: FloatArray, offset: Int)
    external fun getTransformComponent(entity: Int): Long
    external fun setTransformComponent(entity: Int, f: FloatArray)
}