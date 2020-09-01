package com.klmn.misery

import android.app.Activity
import android.view.MotionEvent
import com.klmn.misery.math.*
import com.klmn.misery.render.Material
import com.klmn.misery.render.Mesh
import com.klmn.misery.render.Shader
import com.klmn.misery.render.Texture
import com.klmn.misery.update.*
import kotlin.math.max
import kotlin.math.min
import kotlin.random.Random

/**
 * ಠ^ಠ.
 * Created by Michael on 1/18/2019.
 */
open class Game(val activity: Activity)
{
    val inputBuffer = mutableSetOf<MotionEvent>()

    open fun init() {}
    open fun onViewChanged(width: Int, height: Int) {}
}

class PigGame(activity: Activity) : Game(activity)
{
    override fun onViewChanged(width: Int, height: Int) { MiseryJNI.setViewSize(width, height) }

    override fun init() {
        val mesh = Mesh("pig.obj")
        val material = Material()
        material.diffuse = Texture(activity.assets, "pig.png")
        material.shader = Shader("vertex.glsl", "fragment.glsl")
        val transform = Transform(
                scale = Vec3f(0.05f),
                rotation = Quaternion.rotation(Vec3f.RIGHT, 0.5f)
        )
        val aabb = AABB(Vec3f(-500f), Vec3f(500f));

        var touchID = -1
        var touchPos: Vec2f? = null
        fun createEntity() = Entity(
                "mesh" to mesh,
                "material" to material,
                "transform" to transform.copy(translation = Vec3f(
                        Random.nextFloat() * 10f - 5f,
                        Random.nextFloat() * 10f - 5f,
                        -20f
                )),
                "aabb" to aabb,
                "temp" to 0
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
                                val pigTransform = this["transform", Transform::class]!!
                                pigTransform.translation += Vec3f(
                                        (touchPos!!.x - newPos.x) * 0.02f,
                                        (touchPos!!.y - newPos.y) * 0.01f,
                                        0f
                                )
                                fun clamp(value: Float, bottom: Float, top: Float) = min(top, max(bottom, value))
                                pigTransform.translation =
                                        Vec3f(
                                                clamp(pigTransform.translation.x, -15f, 15f),
                                                pigTransform.translation.y,
                                                clamp(pigTransform.translation.z, -40f, 2f)
                                        )

                                touchPos = newPos
                            }
                        }
                ),
                "aabb" to aabb
        )

        val velocity = 0.1f
        system("transform") { entity, delta ->
            entity["transform", Transform::class]!!.rotation *= Quaternion.rotation(Vec3f.UP, delta * velocity * Math.PI.toFloat())
        }

        system("movementControl") { entity, _ ->
            inputBuffer.forEach { entity["movementControl", TouchControls::class]!!.onTouchEvent(entity, it) }
        }

        system("_taabb") { entity, _ ->

        }

        interaction(arrayOf("movementControl"), arrayOf("temp")) { a, b, _ ->
            println("omfg")
            b.destroy()
            createEntity()
        }
    }
}
