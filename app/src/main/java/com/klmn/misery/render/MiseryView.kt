package com.klmn.misery.render

import android.annotation.SuppressLint
import android.opengl.GLES30.*
import android.opengl.GLSurfaceView
import android.view.MotionEvent
import com.klmn.misery.Game
import com.klmn.misery.jni.MiseryJNI
import java.lang.System.nanoTime
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

/**
 * ಠ^ಠ.
 * Created by Michael on 12/14/2018.
*/
private const val SECOND = 1000000000L
private const val MILLISECOND = 1000000L

/* TODO:
    -separate MiseryView from GLThread, render gl content to a buffer, render android ui on top
    -perhaps move ALL code except for JNI to native
    -figure out ui & input management, should be separate from the ecs and as close as possible to
    regular android app ui
    -native code- urgent organization, try avoiding global variables (have create and set methods
    to avoid increased JNI traffic)
    -debug mesh loading code- some serious bugginess there
    -give Game class some actual options
    -a proper render & physics engines
    -turn this into a library and export
 */

@SuppressLint("ViewConstructor")
class MiseryView(val game: Game) : GLSurfaceView(game.activity), GLSurfaceView.Renderer
{
    init {
        setEGLContextClientVersion(3)
        setEGLConfigChooser(8, 8, 8, 8, 16, 0)
        setRenderer(this)

        MiseryJNI.startThread()
    }

    private val FRAME_CAP = 60;

    private var fps = 0
    private var last = 0L
    private var timer = 0L
    private val frameTime = SECOND / FRAME_CAP
    override fun onDrawFrame(gl: GL10) {
        var now = nanoTime()
        if (timer >= SECOND) {
            println("fps: $fps")
            timer = 0L
            fps = 0
        }
        if (now - last < frameTime) {
            Thread.sleep((frameTime - now + last) / MILLISECOND)
            now = nanoTime()
        }

        glClear(GL_COLOR_BUFFER_BIT + GL_DEPTH_BUFFER_BIT)
        if (last != 0L) MiseryJNI.updateECS((now - last).toFloat() / SECOND)

        fps++
        timer += now - last
        last = now
    }

    override fun onSurfaceChanged(gl: GL10?, width: Int, height: Int) = game.onViewChanged(width, height)

    override fun onSurfaceCreated(p0: GL10?, p1: EGLConfig?) {
        glFrontFace(GL_CW)
        glCullFace(GL_BACK)
        glEnable(GL_CULL_FACE)
        glEnable(GL_DEPTH_TEST)

        glClearColor(0f, 0f, 0f, 1f)

        game.init()
    }

    override fun onTouchEvent(event: MotionEvent): Boolean = game.inputBuffer.add(event)
}
