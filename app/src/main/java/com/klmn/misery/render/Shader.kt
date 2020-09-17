package com.klmn.misery.render

import android.opengl.GLES30.*
import com.klmn.misery.jni.MiseryJNI
import com.klmn.misery.math.Mat4f
import com.klmn.misery.math.Vec2f
import com.klmn.misery.math.Vec3f

/**
 * ಠ^ಠ.
 * Created by Michael on 12/22/2018.
 */
class Shader(val pointer: Long)
{
    private val uniforms = mutableMapOf<String, Int>()

    constructor(vertex: String, fragment: String) : this(MiseryJNI.createProgram(vertex, fragment))

    // fixme convert these methods to JNI calls
    fun bind() = glUseProgram(0)
    fun unbind() = glUseProgram(0)

    private fun uniformID(name: String): Int {
        check(name in uniforms.keys) { "no such uniform $name. note that unused uniforms are removed" }
        return uniforms[name]!!
    }
    fun loadUniform(name: String, value: Int) = glUniform1i(uniformID(name), value)
    fun loadUniform(name: String, value: Float) = glUniform1f(uniformID(name), value)
    fun loadUniform(name: String, value: Vec2f) = glUniform2f(uniformID(name), value.x, value.y)
    fun loadUniform(name: String, value: Vec3f) = glUniform3f(uniformID(name), value.x, value.y, value.z)
    fun loadUniform(name: String, value: Mat4f) = glUniformMatrix4fv(uniformID(name), 1, true, value.toFloatArray(), 0)
    fun loadUniform(name: String, loadFunction: (Int) -> Unit) = loadFunction(uniformID(name))
}
