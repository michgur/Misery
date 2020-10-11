package com.klmn.misery.examples

import android.app.Activity
import android.view.MotionEvent
import com.klmn.misery.Game
import com.klmn.misery.math.Quaternion
import com.klmn.misery.math.Vec3f
import com.klmn.misery.render.Material
import com.klmn.misery.render.Mesh
import com.klmn.misery.render.Shader
import com.klmn.misery.render.Texture
import com.klmn.misery.update.*
import kotlin.random.Random

class FlappyPig(activity: Activity) : Game(activity) {
    private var speed = 0f

    private val player = mapOf(
            "mesh" to Mesh("pig.obj"),
            "material" to Material(
                    shader = Shader("vertex.glsl", "fragment.glsl"),
                    diffuse = Texture("pig.png")
            ),
            "transform" to Transform(
                    translation = Vec3f.BACK * 8f,
                    rotation = Quaternion.rotation(Vec3f.UP, -.6f),
                    scale = Vec3f(.02f)
            ),
            "aabb" to AABB(Vec3f(-18f, -1f, -88f), Vec3f(27f, 73f, 108f)),
            "motion" to Motion(),
            "controls" to TouchControls(
                    MotionEvent.ACTION_DOWN to {
                        if (speed == 0f) {
                            removePillars()
                            this["transform", Transform::class]!!.translation = Vec3f.BACK * 8f
                            this["motion", Motion::class]!!.velocity = Vec3f(0f)
                            speed = 3f
                        }
                        else this["motion", Motion::class]!!.velocity = Vec3f.UP * 20f
                    },
            )
    )
    private val pillar = mapOf(
            "mesh" to Mesh("cube.obj"),
            "material" to Material(
                    shader = Shader("vertex.glsl", "fragment.glsl"),
                    diffuse = Texture("pillar_diffuse.png")
            ),
            "aabb" to AABB(Vec3f(-.5f), Vec3f(.5f)),
            "pillar" to true
    )
    private val sky = mapOf(
            "mesh" to Mesh("cube.obj"),
            "material" to Material(
                    shader = Shader("vertex.glsl", "fragment.glsl"),
                    diffuse = Texture("skyTexture.jpg")
            ),
            "transform" to Transform(
                    translation = Vec3f.BACK * 12f,
                    scale = Vec3f(40.1f, 40f, 0f)
            ),
            "background" to true
    )

    private val pillars = mutableListOf<Entity>()
    private fun createObstacle() {
        val gap = Random.nextInt(-10, 10).toFloat()
        val gapSize = 10f
        pillars.add(entity(pillar, "transform" to Transform(
                translation = Vec3f(-15f, (gap - 15f - gapSize) / 2f, -8f),
                scale = Vec3f(1f, 15f + gap, 1f)
        )))
        pillars.add(entity(pillar, "transform" to Transform(
                translation = Vec3f(-15f, (gap + 15f + gapSize) / 2f, -8f),
                scale = Vec3f(1f, 15f - gap, 1f)
        )))
    }
    private fun removePillar(pillar: Entity) { if (pillars.remove(pillar)) pillar.destroy() }
    private fun removePillars() {
        pillars.forEach { it.destroy() }
        pillars.clear()
    }

    override fun init() {
        entity(player)
        for (i in 0..1) entity(sky)["transform", Transform::class]!!.translation.x = i * 40f
        pillar.isEmpty()

        // process inputs
        system("controls") { entity, _ ->
            if (!inputLock.isLocked) {
                inputBuffer.forEach { entity["controls", TouchControls::class]!!.onTouchEvent(entity, it) }
                inputBuffer.clear()
            }
        }

        var timer = 0f
        var nextPillar = 0
        system("background") { entity, delta ->
            val transform = entity["transform", Transform::class]!!
            if (transform.translation.x > 40f) transform.translation.x = -40f
            transform.translation += Vec3f.RIGHT * speed * delta
            if (speed > 0f) {
                timer += delta
                if (timer > nextPillar) {
                    createObstacle()
                    nextPillar = Random.nextInt(3, 10)
                    timer = 0f
                }
            }
        }

        system("pillar") { entity, delta ->
            val transform = entity["transform", Transform::class]!!
            transform.translation += Vec3f.RIGHT * speed * delta
            if (transform.translation.x > 15f) removePillar(entity)
        }

        val gravity = Vec3f.DOWN * 12f
        system("motion", "controls") { entity, delta ->
            if (speed == 0f) return@system
            val motion = entity["motion", Motion::class]!!
            val transform = entity["transform", Transform::class]!!
            motion.velocity += gravity * speed * delta
            transform.rotation = (transform.rotation * Quaternion.rotation(Vec3f.UP, delta)).normalized

            if (transform.translation.y > 25) motion.velocity *= -1f // hit ceiling
            if (transform.translation.y < -15) speed = 0f
        }

        interaction(arrayOf("controls"), arrayOf("pillar")) { e, p, _ ->
            e["motion", Motion::class]!!.velocity = Vec3f(0f)
            speed = 0f
        }
    }
}