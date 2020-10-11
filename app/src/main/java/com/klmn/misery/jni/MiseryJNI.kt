package com.klmn.misery.jni

import android.content.res.AssetManager
import android.graphics.Bitmap
import android.view.Surface
import android.view.SurfaceHolder
import com.klmn.misery.update.Entity
import java.nio.ByteBuffer

object MiseryJNI {
    init { System.loadLibrary("misery-native-lib") }

    external fun loadMesh(file: String): Long
    external fun drawMesh(pointer: Long)
    external fun createProgram(vertexFile: String, fragmentFile: String): Long
    external fun loadTexture(file: String): Long

    external fun updateECS(delta: Float)
    external fun clearECS()

    external fun addSystem(types: Array<String>, apply: (Entity, Float) -> Unit)
    external fun addInteraction(activeTypes: Array<String>, passiveTypes: Array<String>,
                                apply: (Entity, Entity, Float) -> Unit)
    external fun createEntity(wrapper: Entity): Int
    external fun removeEntity(entity: Int)

    // consider utilizing ByteBuffers when passing large chunks of data between Java/C++.
    // tho I'm not sure keeping track of all ECS calls and combining them to a single ByteBuffer is worth it.
    // the engine is designed so Java side's use of the data is minimal
    external fun putComponent(entity: Int, type: String, value: Any)
    external fun putIntComponent(entity: Int, type: String, value: Int)
    external fun putLongComponent(entity: Int, type: String, value: Long)
    external fun getComponent(entity: Int, type: String): Any
    external fun removeComponent(entity: Int, type: String)
    external fun getComponentPointer(entity: Int, type: String): Long
    external fun getTransformComponent(entity: Int): Long
    external fun putTransformComponent(entity: Int, f: FloatArray)
    external fun putAABBComponent(entity: Int, aabb: FloatArray)
    external fun putMotionComponent(entity: Int, motion: FloatArray)
    external fun putMaterialComponent(entity: Int, material: Int)
    external fun getMaterialComponent(entity: Int): Entity

    external fun getFloat(pointer: Long, offset: Int): Float
    external fun setFloat(pointer: Long, f: Float, offset: Int)
    external fun getFloats(pointer: Long, count: Int, offset: Int): FloatArray
    external fun setFloats(pointer: Long, f: FloatArray, offset: Int)

    external fun getCameraTransform(): Long
    external fun setCameraTransform(camera: Long)

    external fun startRenderThread()
    external fun killRenderThread()
    external fun setSurface(surface: Surface, assets: AssetManager)
    external fun releaseSurface()
}