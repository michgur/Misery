package com.klmn.misery.math

import com.klmn.misery.jni.NativeFloatDelegate
import kotlin.math.sqrt

/**
 * ಠ^ಠ.
 * Created by Michael on 12/14/2018.
 */
sealed class Vec3f {
    abstract var x: Float
    abstract var y: Float
    abstract var z: Float

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

fun Vec3f(x: Float, y: Float, z: Float) = _Vec3f(x, y, z)
fun Vec3f(f: Float = 0f) = _Vec3f(f, f, f)
fun Vec3f(f: FloatArray) = _Vec3f(f[0], f[1], f[2])
fun Vec3f(v: Vec2f, z: Float) = _Vec3f(v.x, v.y, z)
fun Vec3f(f: Float, v: Vec2f) = _Vec3f(f, v.x, v.y)
data class _Vec3f(override var x: Float, override var y: Float, override var z: Float) : Vec3f()

class NativeVec3f(pointer: Long, index: Int) : Vec3f() {
    override var x: Float by NativeFloatDelegate(pointer, index)
    override var y: Float by NativeFloatDelegate(pointer, index + 1)
    override var z: Float by NativeFloatDelegate(pointer, index + 2)
}
