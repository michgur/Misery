package com.klmn.misery.examples

import android.app.Activity
import android.view.MotionEvent
import com.klmn.misery.Game
import com.klmn.misery.math.ImmutableVec3f
import com.klmn.misery.math.Vec3f
import com.klmn.misery.render.Material
import com.klmn.misery.render.Mesh
import com.klmn.misery.render.Shader
import com.klmn.misery.render.Texture
import com.klmn.misery.update.*

class FlappyPig(activity: Activity) : Game(activity) {
    var speed = 0f;

    private val pillar: Map<String, Any> by lazy { mapOf(
            "mesh" to Mesh("pillar.obj"),
            "material" to Material(
                    shader = Render.shader,
                    diffuse = Texture(activity.assets, "pillarTexture.jpg")
            ),
            "transform" to Transform(translation = Vec3f(-15f, -25f, -18f), scale = Vec3f(6f)),
            "aabb" to AABB(Vec3f(-.1f, 0f, -.6f), Vec3f(.1f, 4f, .15f)),
            "pillar" to true
    )}
    private val pillars = mutableListOf<Entity>()
    private fun createPillar() = pillars.add(Entity(pillar))
    private fun removePillars() {
        pillars.forEach { it.destroy() }
        pillars.clear()
    }

    private object Render {
        val shader: Shader by lazy { Shader("vertex.glsl", "fragment.glsl") }
        val cubeMesh: Mesh by lazy { Mesh("cube.obj") }
    }
    private fun createPlayer() = Entity (
            "mesh" to Render.cubeMesh,
            "material" to Material(
                    shader = Render.shader,
                    diffuse = Texture(activity.assets, "cubeTexture.jpg")
            ),
            "transform" to Transform(translation = Vec3f.BACK * 18f),
            "aabb" to AABB(Vec3f(-.5f), Vec3f(.5f)),
            "motion" to Motion(),
            "controls" to TouchControls(
                    MotionEvent.ACTION_DOWN to { event ->
                        if (speed == 0f) {
                            this["transform", Transform::class]!!.translation = ImmutableVec3f.BACK * 18f

                            createPillar()
                            speed = 3f
                        }
                        else this["motion", Motion::class]!!.acceleration = Vec3f.UP * 45f
                    },
            )
    )
    private fun createSky() {
        for (i in 0..1) Entity(
            "mesh" to Render.cubeMesh,
            "material" to Material(
                    shader = Render.shader,
                    diffuse = Texture(activity.assets, "skyTexture.jpg")
            ),
            "transform" to Transform(
                    translation = Vec3f(i * 40f, 0f, -20f),
                    scale = Vec3f(40.1f, 40f, 0f)
            ),
            "background" to true
        )
    }

    override fun init() {
        createPlayer()
        createSky()

        // process inputs
        system("controls") { entity, _ ->
            inputBuffer.forEach { entity["controls", TouchControls::class]!!.onTouchEvent(entity, it) }
        }

        system("background") { entity, delta ->
            val transform = entity["transform", Transform::class]!!
            if (transform.translation.x > 40f)
                transform.translation = ImmutableVec3f(-40f, transform.translation.yz)
            transform.translation += Vec3f.RIGHT * speed * delta
        }

        system("pillar") { entity, delta ->
            val transform = entity["transform", Transform::class]!!
            transform.translation += Vec3f.RIGHT * speed * delta
            if (transform.translation.x > 15f) {
                entity.destroy()
                createPillar()
            }
        }

        val gravity = Vec3f.DOWN * 15f
        system("motion", "controls") { entity, delta ->
            if (speed == 0f) return@system
            val motion = entity["motion", Motion::class]!!
            val transform = entity["transform", Transform::class]!!
            motion.acceleration += gravity * speed * delta

            if (transform.translation.y > 25) motion.acceleration *= -1f // hit ceiling
            if (transform.translation.y < -15) speed = 0f
        }

        interaction(arrayOf("controls"), arrayOf("pillar")) { e, p, _ ->
            e["motion", Motion::class]!!.acceleration = Vec3f(0f)
            speed = 0f
            p.destroy()
        }
    }
}