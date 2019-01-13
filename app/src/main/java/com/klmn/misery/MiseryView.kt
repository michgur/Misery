package com.klmn.misery

import android.annotation.SuppressLint
import android.app.Activity
import android.opengl.GLES30.*
import android.opengl.GLSurfaceView
import android.view.MotionEvent
import com.klmn.misery.math.*
import java.lang.System.nanoTime
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10
import kotlin.math.max
import kotlin.math.min
import kotlin.random.Random

/**
 * ಠ^ಠ.
 * Created by Michael on 12/14/2018.
*/
private const val SECOND = 1000000000L

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
class MiseryView(private val activity: Activity) : GLSurfaceView(activity), GLSurfaceView.Renderer
{
    init {
        setEGLContextClientVersion(3)
        setEGLConfigChooser(8, 8, 8, 8, 16, 0)
        setRenderer(this)
    }

    var pigVelocity = 0.007f
    var colorVelocity = 0.02f
    lateinit var pig: Entity
    lateinit var shader: Shader
    var nextColor = Vec3f()
    var color = Vec3f()
    var camera = Camera()
    lateinit var modelRenderer: ModelRenderer
    lateinit var interactionSystem: InteractionSystem

    private var fps = 0
    private var last = 0L
    private var timer = 0L
    private val frameTime = SECOND / 60
    override fun onDrawFrame(gl: GL10) {
        val now = nanoTime()
        if (timer >= SECOND) {
            println("fps: $fps")
            timer = 0L
            fps = 0
        }
        if (now - last < frameTime) return

        color = color * (1f - colorVelocity) + nextColor * colorVelocity
        if ((color - nextColor).length < 0.05f)
            nextColor = Vec3f(Random.nextFloat(), Random.nextFloat(), Random.nextFloat()).normalized
        glClearColor(color.x, color.y, color.z, 1f)

        glClear(GL_COLOR_BUFFER_BIT + GL_DEPTH_BUFFER_BIT)

        modelRenderer.update((now - last).toFloat() / SECOND)
        pig[Transform::class]!!.rotation *= Quaternion.rotation(Vec3f.UP, pigVelocity * Math.PI.toFloat())

        fps++
        timer += now - last
        last = now
    }

    override fun onSurfaceChanged(gl: GL10?, width: Int, height: Int) {
        modelRenderer = ModelRenderer(shader,
                Perspective(Math.PI.toFloat() * 0.5f, width, height, 0.1f, 1000f), camera)
    }

    override fun onSurfaceCreated(p0: GL10?, p1: EGLConfig?) {
        glFrontFace(GL_CW)
        glCullFace(GL_BACK)
        glEnable(GL_CULL_FACE)
        glEnable(GL_DEPTH_TEST)

        glClearColor(0f, 0f, 0f, 1f)

        shader = Shader(activity.assets, "vertex.glsl", "fragment.glsl")
        val mesh = Mesh(activity.assets, "pig.obj")
        val texture = Texture(activity.assets, "pig.png")

        val transform = Transform(
                scale = Vec3f(0.1f),
                translation = Vec3f(0f, -1f, 20f),
                rotation = Quaternion.rotation(Vec3f.LEFT, 0.5f)
        )

        pig = Entity()
        pig.put(Model(mesh, texture))
        pig.put(transform)

        init = true

        val collidable = setOf(Transform::class, AABB::class)
        interactionSystem = InteractionSystem(collidable)
        interactionSystem.addInteraction(Interaction(collidable, collidable) { delta, a, b ->
            println("$a intersects $b (delta $delta)")
        })
    }

    var init = false
    var touch: Vec2f? = null
    override fun onTouchEvent(event: MotionEvent): Boolean {
        if (init) when (event.action) {
            MotionEvent.ACTION_DOWN -> touch = Vec2f(event.x, event.y)
            MotionEvent.ACTION_MOVE -> {
                val delta = (touch ?: return true) - Vec2f(event.x, event.y)

                val transform = pig[Transform::class]!!
                transform.translation += Vec3f(delta.x * -0.02f, 0f, delta.y * 0.01f)
                fun clamp(value: Float, bottom: Float, top: Float) = min(top, max(bottom, value))
                transform.translation =
                        Vec3f(clamp(transform.translation.x, -15f, 15f), 0f,
                        clamp(transform.translation.z, -2f, 40f))

                touch = Vec2f(event.x, event.y)
            }
        }

        return true
    }
}
