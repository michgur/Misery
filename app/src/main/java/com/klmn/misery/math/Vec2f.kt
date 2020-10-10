package com.klmn.misery.math

import com.klmn.misery.jni.NativeFloatDelegate
import kotlin.math.sqrt

/**
 * ಠ^ಠ.
 * Created by Michael on 12/14/2018.
 */
sealed class Vec2f {
    abstract var x: Float
    abstract var y: Float

    operator fun get(index: Int) = when (index) {
        0 -> x
        1 -> y
        else -> throw IndexOutOfBoundsException()
    }
    operator fun set(index: Int, f: Float) = when (index) {
        0 -> x = f
        1 -> y = f
        else -> throw IndexOutOfBoundsException()
    }

    inline val length get() = sqrt(x * x + y * y)
    inline val normalized get() = this / length

    operator fun plus(other: Vec2f) = Vec2f(x + other.x, y + other.y)
    operator fun minus(other: Vec2f) = Vec2f(x - other.x, y - other.y)
    operator fun times(other: Vec2f) = Vec2f(x * other.x, y * other.y)
    operator fun div(other: Vec2f) = Vec2f(x / other.x, y / other.y)

    operator fun times(f: Float) = Vec2f(x * f, y * f)
    operator fun div(f: Float) = Vec2f(x / f, y / f)

    infix fun dot(other: Vec2f) = x * other.x + y * other.y

    override fun toString() = "($x, $y)"
    fun toFloatArray() = floatArrayOf(x, y)

    companion object {
        val UP      = Vec2f(0f, 1f)
        val DOWN    = Vec2f(0f, -1f)
        val RIGHT   = Vec2f(1f, 0f)
        val LEFT    = Vec2f(-1f, 0f)
    }
}

fun Vec2f(x: Float, y: Float) = _Vec2f(x, y)
fun Vec2f(f: Float) = _Vec2f(f, f)
data class _Vec2f(override var x: Float, override var y: Float) : Vec2f()

class NativeVec2f(pointer: Long, index: Int) :  Vec2f() {
    override var x by NativeFloatDelegate(pointer, index)
    override var y by NativeFloatDelegate(pointer, index + 1)
}
