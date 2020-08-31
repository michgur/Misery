package com.klmn.misery

import android.content.res.AssetManager
import com.klmn.misery.update.Entity
import java.lang.System

object MiseryJNI
{
    init { System.loadLibrary("misery-native-lib") }

    external fun setNativeAssetManager(assetManager: AssetManager)
    external fun loadMesh(file: String, ext: String): Long
    external fun drawMesh(pointer: Long)
    external fun createProgram(vertexFile: String, fragmentFile: String): Int

    external fun updateECS(delta: Float)
    external fun clearECS()

    external fun addSystem(types: Array<String>, apply: (Entity, Float) -> Unit)
    external fun addInteraction(activeTypes: Array<String>, passiveTypes: Array<String>,
                                apply: (Entity, Entity, Float) -> Unit)
    external fun createEntity(wrapper: Entity): Int
    external fun removeEntity(entity: Int)

    external fun putComponent(entity: Int, type: String, value: Any)
    external fun putIntComponent(entity: Int, type: String, value: Int)
    external fun putLongComponent(entity: Int, type: String, value: Long)
    external fun getComponent(entity: Int, type: String): Any
    external fun removeComponent(entity: Int, type: String)
    external fun getTransformComponent(entity: Int): Long
    external fun setTransformComponent(entity: Int, f: FloatArray)
    external fun setAABBComponent(entity: Int, aabb: FloatArray)
    external fun setMaterialComponent(entity: Int, material: Int)

    external fun getMaterialComponent(entity: Int): Entity
    external fun getFloat(pointer: Long, offset: Int): Float
    external fun setFloat(pointer: Long, f: Float, offset: Int)
    external fun getFloats(pointer: Long, count: Int, offset: Int): FloatArray

    external fun setFloats(pointer: Long, f: FloatArray, offset: Int)
    external fun getCameraTransform(): Long
    external fun setViewSize(width: Int, height: Int)
}