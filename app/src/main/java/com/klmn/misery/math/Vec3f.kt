package com.klmn.misery.math

import kotlin.math.sqrt

/**
 * ಠ^ಠ.
 * Created by Michael on 12/14/2018.
 */
data class Vec3f(var x: Float, var y: Float, var z: Float)
{
    constructor(f: Float = 0f) : this(f, f, f)
    constructor(f: FloatArray) : this(f[0], f[1], f[2])

    operator fun get(index: Int) = when (index) {
        0 -> x
        1 -> y
        2 -> z
        else -> throw IndexOutOfBoundsException()
    }
    operator fun set(index: Int, f: Float) = when (index) {
        0 -> x = f
        1 -> y = f
        2 -> z = f
        else -> throw IndexOutOfBoundsException()
    }

    inline val length get() = sqrt(x * x + y * y + z * z)
    inline val normalized get() = this / length

    operator fun plus(other: Vec3f) = Vec3f(x + other.x, y + other.y, z + other.z)
    operator fun minus(other: Vec3f) = Vec3f(x - other.x, y - other.y, z - other.z)
    operator fun times(other: Vec3f) = Vec3f(x * other.x, y * other.y, z * other.z)
    operator fun div(other: Vec3f) = Vec3f(x / other.x, y / other.y, z / other.z)

    operator fun times(f: Float) = Vec3f(x * f, y * f, z * f)
    operator fun div(f: Float) = Vec3f(x / f, y / f, z / f)

    infix fun dot(other: Vec3f) = x * other.x + y * other.y + z * other.z
    infix fun cross(other: Vec3f) = Vec3f(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
    )

    override fun toString() = "($x, $y, $z)"
    fun toFloatArray() = floatArrayOf(x, y, z)

    companion object {
        val UP      = Vec3f(0f, 1f, 0f)
        val DOWN    = Vec3f(0f, -1f, 0f)
        val RIGHT   = Vec3f(1f, 0f, 0f)
        val LEFT    = Vec3f(-1f, 0f, 0f)
        val FORWARD = Vec3f(0f, 0f, 1f)
        val BACK    = Vec3f(0f, 0f, -1f)
    }
}
