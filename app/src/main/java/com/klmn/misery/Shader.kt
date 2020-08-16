package com.klmn.misery

import android.opengl.GLES30.*
import com.klmn.misery.math.Mat4f
import com.klmn.misery.math.Vec2f
import com.klmn.misery.math.Vec3f

/**
 * ಠ^ಠ.
 * Created by Michael on 12/22/2018.
 */
class Shader(vertexSource: String, fragmentSource: String)
{
    private val id: Int = MiseryJNI.createProgram(vertexSource, fragmentSource)
    private val uniforms = mutableMapOf<String, Int>()

    init {
        bind()
        val count = IntArray(2)
        glGetProgramiv(id, GL_ACTIVE_ATTRIBUTES, count, 0)
        glGetProgramiv(id, GL_ACTIVE_UNIFORMS, count, 1)

        val sizeAndType = IntArray(2)
        /** for (i in 0 until count[0]) {
        val name = glGetActiveAttrib(id, i, sizeAndType, 0, sizeAndType, 1)
        }*/
        count[1] = 100
        for (i in 0 until count[1]) {
            val name = glGetActiveUniform(id, i, sizeAndType, 0, sizeAndType, 1)
            uniforms[name] = glGetUniformLocation(id, name)
        }
        unbind()
    }

    fun bind() = glUseProgram(id)
    fun unbind() = glUseProgram(0)

    private fun uniformID(name: String): Int {
        check(name in uniforms.keys) { "no such uniform $name. note that unused uniforms are removed" }
        return uniforms[name]!!
    }
    // these assume Shader is bound
    fun loadUniform(name: String, value: Int) = glUniform1i(uniformID(name), value)
    fun loadUniform(name: String, value: Float) = glUniform1f(uniformID(name), value)
    fun loadUniform(name: String, value: Vec2f) = glUniform2f(uniformID(name), value.x, value.y)
    fun loadUniform(name: String, value: Vec3f) = glUniform3f(uniformID(name), value.x, value.y, value.z)
    fun loadUniform(name: String, value: Mat4f) = glUniformMatrix4fv(uniformID(name), 1, true, value.toFloatArray(), 0)
    fun loadUniform(name: String, loadFunction: (Int) -> Unit) = loadFunction(uniformID(name))
}
