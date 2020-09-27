package com.klmn.misery.render

import android.content.Context
import android.opengl.EGLContext
import android.opengl.GLES20
import android.opengl.*
import android.opengl.EGL14.*
import android.view.SurfaceHolder
import android.view.SurfaceView
import com.klmn.misery.jni.MiseryJNI
import java.nio.ByteBuffer
import java.nio.ByteOrder
import java.nio.FloatBuffer
import kotlin.concurrent.thread


class MiserySurfaceView(context: Context) : SurfaceView(context), SurfaceHolder.Callback {
    init { holder.addCallback(this) }

    override fun surfaceCreated(holder: SurfaceHolder) {}

    override fun surfaceChanged(holder: SurfaceHolder, format: Int, width: Int, height: Int) {
        thread {
            MiseryJNI.initEGL(holder.surface)
            val t = Test()
            while (true) {
                Thread.sleep(1)
                GLES20.glClearColor(1f, 0f, 0f, 1f)
                GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT)
                t.draw()
                MiseryJNI.swapBuffers()
            }
        }
    }

    override fun surfaceDestroyed(holder: SurfaceHolder) {}

//    private var configs = arrayOfNulls<EGLConfig?>(1)
//    private lateinit var display: EGLDisplay
//    private lateinit var context: EGLContext
//    private var surface: EGLSurface? = null
//
//    private fun initializeEGL(holder: SurfaceHolder) {
//        display = eglGetDisplay(EGL_DEFAULT_DISPLAY)
//        if (display === EGL_NO_DISPLAY) throw RuntimeException("Error: eglGetDisplay() Failed " + GLUtils.getEGLErrorString(eglGetError()))
//        val version = IntArray(2)
//        if (!eglInitialize(display, version, 0, version, 1))
//            throw RuntimeException("Error: eglInitialize() Failed " + GLUtils.getEGLErrorString(eglGetError()))
//        val configsCount = IntArray(1)
//        val configSpec = intArrayOf(
//                EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
//                EGL_RED_SIZE, 8,
//                EGL_GREEN_SIZE, 8,
//                EGL_BLUE_SIZE, 8,
//                EGL_ALPHA_SIZE, 8,
//                EGL_DEPTH_SIZE, 0,
//                EGL_STENCIL_SIZE, 0,
//                EGL_NONE
//        )
//        require(eglChooseConfig(display, configSpec, 0,
//                configs, 0, 1, configsCount, 0) && configsCount[0] != 0)
//        { "Error: eglChooseConfig() Failed " + GLUtils.getEGLErrorString(eglGetError()) }
//        if (configs[0] == null) throw RuntimeException("Error: eglConfig() not Initialized")
//        val attrib_list = intArrayOf(0x3098, 3, EGL_NONE)
//        context = eglCreateContext(display, configs[0], EGL_NO_CONTEXT, attrib_list, 0)
//
//        surface = eglCreateWindowSurface(display, configs[0], holder, intArrayOf(EGL_NONE), 0)
//        if (surface == null || surface === EGL_NO_SURFACE) {
//            val error = eglGetError()
//            if (error == EGL_BAD_NATIVE_WINDOW)
//                error("Error: createWindowSurface() Returned EGL_BAD_NATIVE_WINDOW.")
//            throw RuntimeException("Error: createWindowSurface() Failed " + GLUtils.getEGLErrorString(error))
//        }
//        if (!eglMakeCurrent(display, surface, surface, context))
//            throw RuntimeException("Error: eglMakeCurrent() Failed " + GLUtils.getEGLErrorString(eglGetError()))
//        val widthResult = IntArray(1)
//        val heightResult = IntArray(1)
//        eglQuerySurface(display, surface, EGL_WIDTH, widthResult, 0)
//        eglQuerySurface(display, surface, EGL_HEIGHT, heightResult, 0)
//        println("EGL Surface Dimensions:${widthResult[0]} ${heightResult[0]}")
//    }

    class Test {
        private val vertexBuffer: FloatBuffer
        private val color: FloatArray
        private val program: Int

        private val vertexShaderCode =
            "attribute vec4 vPosition;" +
            "void main() {" +
            "   gl_Position = vPosition;" +
            "}";
        private val fragmentShaderCode =
            "precision mediump float;" +
            "uniform vec4 vColor;" +
            "void main() {" +
            "   gl_FragColor = vColor;" +
            "}";

        private fun loadShader(type: Int, shaderCode: String): Int {
            val shader = GLES20.glCreateShader(type)
            GLES20.glShaderSource(shader, shaderCode)
            GLES20.glCompileShader(shader)
            return shader
        }
        init {
            val triangleCoords = floatArrayOf(0.0f, 0.6f, 0.0f, -0.5f, -0.3f, 0.0f, 0.5f, -0.3f, 0.0f)
            vertexBuffer = ByteBuffer.allocateDirect(triangleCoords.size * 4)
                    .order(ByteOrder.nativeOrder())
                    .asFloatBuffer()
                    .put(triangleCoords)
            vertexBuffer.position(0)
            color = floatArrayOf(0.63671875f, 0.76953125f, 0.22265625f, 1.0f)

            val vertexShader = loadShader(GLES20.GL_VERTEX_SHADER, vertexShaderCode)
            val fragmentShader = loadShader(GLES20.GL_FRAGMENT_SHADER, fragmentShaderCode)
            program = GLES20.glCreateProgram()
            GLES20.glAttachShader(program, vertexShader)
            GLES20.glAttachShader(program, fragmentShader)
            GLES20.glLinkProgram(program)
        }

        fun draw() {
            GLES20.glUseProgram(program);
            val positionHandle = GLES20.glGetAttribLocation(program, "vPosition")
            GLES20.glEnableVertexAttribArray(positionHandle);
            GLES20.glVertexAttribPointer(positionHandle, 3,
                    GLES20.GL_FLOAT, false, 12, vertexBuffer);
            val colorHandle = GLES20.glGetUniformLocation(program, "vColor")
            GLES20.glUniform4fv(colorHandle, 1, color, 0);
            GLES20.glDrawArrays(GLES20.GL_TRIANGLES, 0, 3);
            GLES20.glDisableVertexAttribArray(positionHandle);
        }
    }
}