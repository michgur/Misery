package com.klmn.misery

import android.app.Activity
import android.opengl.GLES30.*
import android.view.MotionEvent
import com.klmn.misery.jni.MiseryJNI
import com.klmn.misery.math.*
import com.klmn.misery.render.Material
import com.klmn.misery.render.Mesh
import com.klmn.misery.render.Shader
import com.klmn.misery.render.Texture
import com.klmn.misery.update.*
import java.nio.FloatBuffer
import kotlin.math.acos
import kotlin.math.pow
import kotlin.random.Random

/**
 * ಠ^ಠ.
 * Created by Michael on 1/18/2019.
 */
abstract class Game(val activity: Activity)
{
    val inputBuffer = mutableSetOf<MotionEvent>()

    abstract fun init()
    open fun onViewChanged(width: Int, height: Int) { MiseryJNI.setViewSize(width, height) }
}

class PigGame(activity: Activity) : Game(activity)
{
    private var projection = Mat4f()
    override fun onViewChanged(width: Int, height: Int) {
        super.onViewChanged(width, height)
        projection = Mat4f.perspective(acos(0f), width.toFloat(), height.toFloat(), 0.1f, 1000f)
    }

    override fun init() {
        val mesh = Mesh("pig.obj")
        val material = Material()
        material.diffuse = Texture(activity.assets, "pig.png")
        material.shader = Shader("vertex.glsl", "fragment.glsl")
        val transform = Transform(
                scale = Vec3f(0.05f),
                rotation = Quaternion.rotation(Vec3f.RIGHT, 0.5f)
        )
        val aabb = AABB(Vec3f(-25f, 10f, -85f), Vec3f(25f, 40f, 90f));

        var touchID = -1
        var touchPos: Vec2f? = null
        var lastInput = Vec2f(0f)
        fun createEntity() = Entity(
                "mesh" to mesh,
                "material" to material,
                "transform" to transform.copy(translation = Vec3f(
                        Random.nextFloat() * 10f - 5f,
                        Random.nextFloat() * 40f - 20f,
                        -20f
                )),
                "aabb" to aabb,
                "motion" to Motion()
        )
        createEntity()
        createEntity()
        Entity(
                "mesh" to mesh,
                "material" to material,
                "transform" to transform.copy(translation = Vec3f(
                        Random.nextFloat() * 10f - 5f,
                        Random.nextFloat() * 10f - 5f,
                        -20f
                )),
                "motion" to Motion(),
                "movementControl" to TouchControls(
                        MotionEvent.ACTION_DOWN to { event ->
                            touchPos = Vec2f(event.x, event.y)
                            touchID = event.getPointerId(0)
                        },
                        MotionEvent.ACTION_MOVE to { event ->
                            if (touchPos != null && event.findPointerIndex(touchID) >= 0) {
                                val newPos = Vec2f(
                                        event.getX(event.findPointerIndex(touchID)),
                                        event.getY(event.findPointerIndex(touchID))
                                )

                                val motion = this["motion", Motion::class]!!
                                val input = Vec2f(touchPos!!.x - newPos.x, touchPos!!.y - newPos.y)
                                motion.acceleration += Vec3f(input - lastInput, 0f)
                                lastInput = input

                                touchPos = newPos
                            }
                        }
                ),
                "aabb" to aabb
        )

        system("movementControl", "motion") { entity, _ ->
            inputBuffer.forEach { entity["movementControl", TouchControls::class]!!.onTouchEvent(entity, it) }
        }

//        createBBRenderer()

        interaction(arrayOf("motion"), arrayOf("motion")) { a, b, delta ->
            val aM = a["motion", Motion::class]!!
            aM.velocity *= -1.1f
        }
    }

    private fun createBBRenderer() {
        val vertices = floatArrayOf(
                1f, 1f, 1f, 1f, 1f, -1f,
                -1f, 1f, 1f, -1f, 1f, -1f,
                1f, -1f, 1f, 1f, -1f, -1f,
                -1f, -1f, 1f, -1f, -1f, -1f,
                1f, 1f, 1f, 1f, -1f, 1f,
                1f, 1f, -1f, 1f, -1f, -1f,
                -1f, 1f, 1f, -1f, -1f, 1f,
                -1f, 1f, -1f, -1f, -1f, -1f,
                1f, 1f, 1f, -1f, 1f, 1f,
                1f, 1f, -1f, -1f, 1f, -1f,
                1f, -1f, 1f, -1f, -1f, 1f,
                1f, -1f, -1f, -1f, -1f, -1f
        )
        val buffers = intArrayOf(0, 0)
        glGenVertexArrays(1, buffers, 0)
        glGenBuffers(1, buffers, 1)
        glBindVertexArray(buffers[0])
        glBindBuffer(GL_ARRAY_BUFFER, buffers[1])
        glBufferData(GL_ARRAY_BUFFER, 4 * vertices.size, FloatBuffer.wrap(vertices), GL_STATIC_DRAW)
        glEnableVertexAttribArray(0)
        glVertexAttribPointer(0, 3, GL_FLOAT, false, 12, 0)
        val bbShader = Shader("vertex_bb.glsl", "fragment_bb.glsl")

        system("transform", "_taabb") { entity, _ ->
            bbShader.bind()
            val transform = entity["transform", Transform::class]!!.matrix
            val floats = MiseryJNI.getFloats(entity["aabb", Long::class]!!, 6, 0)
            transform[0, 0] *= (floats[3] - floats[0]) / 2f
            transform[1, 1] *= (floats[4] - floats[1]) / 2f
            transform[2, 2] *= (floats[5] - floats[2]) / 2f
            bbShader.loadUniform("mvp", projection * transform)
            glBindVertexArray(buffers[0])
            glLineWidth(3.3f)
            glClear(GL_DEPTH_BUFFER_BIT)
            glDrawArrays(GL_LINES, 0, 24)
            glLineWidth(1.0f)
        }
    }
}
