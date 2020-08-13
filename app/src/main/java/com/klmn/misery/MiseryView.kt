package com.klmn.misery

import android.annotation.SuppressLint
import android.opengl.GLES30.*
import android.opengl.GLSurfaceView
import android.view.MotionEvent
import com.klmn.misery.math.Vec3f
import gli_.clear
import java.lang.System.nanoTime
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10
import kotlin.random.Random

/**
 * ಠ^ಠ.
 * Created by Michael on 12/14/2018.
*/
private const val SECOND = 1000000000L
private const val MILLISECOND = 1000000L

/* TODO: a proper engine structure.
    EntityManager for ECS;
    some sort of a RenderContext for gl operations;
    some sort of an InputManager for input events;
    utilities that'll make writing systems easier so the game won't be tempted to write logic inside components
        (for example currently the Mesh class has rendering code.
         RenderContext should get a Mesh and render it, so the logic is out of the component);
    a main class for wrapping the Activity & The GLSurfaceView (game loop);
 */

@SuppressLint("ViewConstructor")
class MiseryView(val game: Game) : GLSurfaceView(game.activity), GLSurfaceView.Renderer
{
    init {
        setEGLContextClientVersion(3)
        setEGLConfigChooser(8, 8, 8, 8, 16, 0)
        setRenderer(this)
    }

    var colorVelocity = 0.02f
    var nextColor = Vec3f()
    var color = Vec3f()

    val FRAME_CAP = 60;

    private var fps = 0
    private var last = 0L
    private var timer = 0L
    private val frameTime = SECOND / FRAME_CAP
    override fun onDrawFrame(gl: GL10) {
        println(MiseryJNI.helloWorld())
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

        color = color * (1f - colorVelocity) + nextColor * colorVelocity
        if ((color - nextColor).length < 0.05f)
            nextColor = Vec3f(Random.nextFloat(), Random.nextFloat(), Random.nextFloat()).normalized
        glClearColor(color.x, color.y, color.z, 1f)

        glClear(GL_COLOR_BUFFER_BIT + GL_DEPTH_BUFFER_BIT)

        game.render((now - last).toFloat() / SECOND)
        game.update((now - last).toFloat() / SECOND)

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
