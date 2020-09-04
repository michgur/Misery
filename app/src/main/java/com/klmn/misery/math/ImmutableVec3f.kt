package com.klmn.misery.math

import kotlin.math.sqrt

/**
 * ಠ^ಠ.
 * Created by Michael on 12/14/2018.
 */
data class ImmutableVec3f(val x: Float, val y: Float, val z: Float)
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

    inline val length get() = sqrt(x * x + y * y + z * z)
    inline val normalized get() = this / length

    operator fun plus(other: Vec3f) = ImmutableVec3f(x + other.x, y + other.y, z + other.z)
    operator fun minus(other: Vec3f) = ImmutableVec3f(x - other.x, y - other.y, z - other.z)
    operator fun times(other: Vec3f) = ImmutableVec3f(x * other.x, y * other.y, z * other.z)
    operator fun div(other: Vec3f) = ImmutableVec3f(x / other.x, y / other.y, z / other.z)
    operator fun plus(other: ImmutableVec3f) = ImmutableVec3f(x + other.x, y + other.y, z + other.z)
    operator fun minus(other: ImmutableVec3f) = ImmutableVec3f(x - other.x, y - other.y, z - other.z)
    operator fun times(other: ImmutableVec3f) = ImmutableVec3f(x * other.x, y * other.y, z * other.z)
    operator fun div(other: ImmutableVec3f) = ImmutableVec3f(x / other.x, y / other.y, z / other.z)

    operator fun times(f: Float) = ImmutableVec3f(x * f, y * f, z * f)
    operator fun div(f: Float) = ImmutableVec3f(x / f, y / f, z / f)

    infix fun dot(other: Vec3f) = x * other.x + y * other.y + z * other.z
    infix fun cross(other: Vec3f) = ImmutableVec3f(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
    )
    infix fun dot(other: ImmutableVec3f) = x * other.x + y * other.y + z * other.z
    infix fun cross(other: ImmutableVec3f) = ImmutableVec3f(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
    )

    override fun toString() = "immutable($x, $y, $z)"
    fun toFloatArray() = floatArrayOf(x, y, z)

    inline val xyz: Vec3f
        get() = Vec3f(x, y, z)
    inline val xy: Vec2f
        get() = Vec2f(x, y)
    inline val xz: Vec2f
        get() = Vec2f(x, z)
    inline val yx: Vec2f
        get() = Vec2f(y, x)
    inline val yz: Vec2f
        get() = Vec2f(y, z)
    inline val zx: Vec2f
        get() = Vec2f(z, x)
    inline val zy: Vec2f
        get() = Vec2f(z, y)

    companion object {
        val UP      = ImmutableVec3f(0f, 1f, 0f)
        val DOWN    = ImmutableVec3f(0f, -1f, 0f)
        val RIGHT   = ImmutableVec3f(1f, 0f, 0f)
        val LEFT    = ImmutableVec3f(-1f, 0f, 0f)
        val FORWARD = ImmutableVec3f(0f, 0f, 1f)
        val BACK    = ImmutableVec3f(0f, 0f, -1f)
    }
}
