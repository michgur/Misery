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
    constructor(v: Vec2f, f: Float) : this(v.x, v.y, f)
    constructor(f: Float, v: Vec2f) : this(f, v.x, v.y)

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
    operator fun plus(other: ImmutableVec3f) = Vec3f(x + other.x, y + other.y, z + other.z)
    operator fun minus(other: ImmutableVec3f) = Vec3f(x - other.x, y - other.y, z - other.z)
    operator fun times(other: ImmutableVec3f) = Vec3f(x * other.x, y * other.y, z * other.z)
    operator fun div(other: ImmutableVec3f) = Vec3f(x / other.x, y / other.y, z / other.z)

    operator fun times(f: Float) = Vec3f(x * f, y * f, z * f)
    operator fun div(f: Float) = Vec3f(x / f, y / f, z / f)

    infix fun dot(other: Vec3f) = x * other.x + y * other.y + z * other.z
    infix fun cross(other: Vec3f) = Vec3f(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
    )
    infix fun dot(other: ImmutableVec3f) = x * other.x + y * other.y + z * other.z
    infix fun cross(other: ImmutableVec3f) = Vec3f(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
    )

    override fun toString() = "($x, $y, $z)"
    fun toFloatArray() = floatArrayOf(x, y, z)

    fun toImmutable() = ImmutableVec3f(x, y, z)

    inline val xyz: Vec3f
        get() = Vec3f(x, y, z)
    inline var xy: Vec2f
        get() = Vec2f(x, y)
        set(value) { x = value.x; y = value.y }
    inline var xz: Vec2f
        get() = Vec2f(x, z)
        set(value) { x = value.x; z = value.y }
    inline var yx: Vec2f
        get() = Vec2f(y, x)
        set(value) { y = value.x; x = value.y }
    inline var yz: Vec2f
        get() = Vec2f(y, z)
        set(value) { y = value.x; z = value.y }
    inline var zx: Vec2f
        get() = Vec2f(z, x)
        set(value) { z = value.x; x = value.y }
    inline var zy: Vec2f
        get() = Vec2f(z, y)
        set(value) { z = value.x; y = value.y }

    companion object {
        val UP      = Vec3f(0f, 1f, 0f)
        val DOWN    = Vec3f(0f, -1f, 0f)
        val RIGHT   = Vec3f(1f, 0f, 0f)
        val LEFT    = Vec3f(-1f, 0f, 0f)
        val FORWARD = Vec3f(0f, 0f, 1f)
        val BACK    = Vec3f(0f, 0f, -1f)
    }
}
