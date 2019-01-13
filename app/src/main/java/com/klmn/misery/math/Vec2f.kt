package com.klmn.misery.math

import kotlin.math.sqrt

/**
 * ಠ^ಠ.
 * Created by Michael on 12/14/2018.
 */
data class Vec2f(var x: Float, var y: Float)
{
    constructor(f: Float = 0f) : this(f, f)

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

    companion object {
        val UP      = Vec2f(0f, 1f)
        val DOWN    = Vec2f(0f, -1f)
        val RIGHT   = Vec2f(1f, 0f)
        val LEFT    = Vec2f(-1f, 0f)
    }
}
