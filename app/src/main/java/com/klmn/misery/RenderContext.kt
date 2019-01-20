package com.klmn.misery

import android.opengl.EGL14.EGL_NO_CONTEXT
import android.opengl.EGL14.eglGetCurrentContext
import android.opengl.GLES20.*

/**
 * ಠ^ಠ.
 * Created by Michael on 1/19/2019.
 */
class RenderContext
{
    val exists get() = eglGetCurrentContext() != EGL_NO_CONTEXT

    fun <T> withContext(action: () -> T) = if(exists) action() else TODO()

    fun createShader(sources: Map<Int, String>) = withContext {
        glCreateProgram().takeUnless { it == 0 }?.also {
            bindShader(it)
            
            unbindShader()
        } ?: TODO("could'nt create program")
    }

    fun getUniformCount(shaderID: Int) = withContext {
        IntArray(1).also { glGetProgramiv(shaderID, GL_ACTIVE_UNIFORMS, it, 0) }[0]
    }
    fun getUniformNameAndLocation(id: Int, index: Int): Pair<String, Int> = withContext {
        glGetActiveUniform(id, index, IntArray(1), 0, IntArray(1), 0).let {
            it to glGetUniformLocation(id, it)
        }
    }
    
    fun bindShader(id: Int) = withContext { glUseProgram(id) }
    fun unbindShader() = withContext { glUseProgram(0) }

    companion object {
        val VERTEX_SHADER = GL_VERTEX_SHADER
        val FRAGMENT_SHADER = GL_FRAGMENT_SHADER
    }
}