package com.klmn.misery

import com.klmn.misery.math.Vec3f
import kotlin.math.pow

/**
 * ಠ^ಠ.
 * Created by Michael on 1/11/2019.
 */
data class Motion(var velocity: Vec3f = Vec3f(), var acceleration: Vec3f = Vec3f())

/* This is how I want the game's code to look like. The user can easily write little shitstickles like this.
 * We don't need a MotionSystem class. We don't need this to be provided by the engine.
 * The engine will only provide the necessary math.
 * This way we: a) allow more customization without adding much boilerplate. b) avoid stupid oop constructs */
//val motionSystem = system(Motion::class, Transform::class) { delta, entity ->
//    val transform = entity[Transform::class]!!
//    val (velocity, acceleration) = entity[Motion::class]!!
//
//    transform.translation = MotionIntegrations.forestRuth(transform.translation, velocity, acceleration, delta)
//}

object MotionIntegrations
{
    fun verlet(pos: Vec3f, velocity: Vec3f, acceleration: Vec3f, delta: Float): Vec3f {
        val res = pos + velocity * (delta * 0.5f)
        return res + (velocity + acceleration * delta) * (delta * 0.5f)
    }

    private val frCoefficient = 1f / (2f - 2f.pow(1f / 3f))
    private val frComplement = 1f - 2f * frCoefficient
    fun forestRuth(pos: Vec3f, velocity: Vec3f, acceleration: Vec3f, delta: Float): Vec3f {
        var res = verlet(pos, velocity, acceleration, delta * frCoefficient)
        res = verlet(res, velocity, acceleration, delta * frComplement)
        return verlet(res, velocity, acceleration, delta * frCoefficient)
    }
}