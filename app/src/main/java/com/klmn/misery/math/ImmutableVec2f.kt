package com.klmn.misery.math

import kotlin.math.sqrt

/**
 * ಠ^ಠ.
 * Created by Michael on 12/14/2018.
 */
data class ImmutableVec2f(val x: Float, val y: Float)
{
    constructor(f: Float = 0f) : this(f, f)

    operator fun get(index: Int) = when (index) {
        0 -> x
        1 -> y
        else -> throw IndexOutOfBoundsException()
    }

    inline val length get() = sqrt(x * x + y * y)
    inline val normalized get() = this / length

    operator fun plus(other: Vec2f) = ImmutableVec2f(x + other.x, y + other.y)
    operator fun minus(other: Vec2f) = ImmutableVec2f(x - other.x, y - other.y)
    operator fun times(other: Vec2f) = ImmutableVec2f(x * other.x, y * other.y)
    operator fun div(other: Vec2f) = ImmutableVec2f(x / other.x, y / other.y)
    operator fun plus(other: ImmutableVec2f) = ImmutableVec2f(x + other.x, y + other.y)
    operator fun minus(other: ImmutableVec2f) = ImmutableVec2f(x - other.x, y - other.y)
    operator fun times(other: ImmutableVec2f) = ImmutableVec2f(x * other.x, y * other.y)
    operator fun div(other: ImmutableVec2f) = ImmutableVec2f(x / other.x, y / other.y)

    operator fun times(f: Float) = ImmutableVec2f(x * f, y * f)
    operator fun div(f: Float) = ImmutableVec2f(x / f, y / f)

    infix fun dot(other: Vec2f) = x * other.x + y * other.y
    infix fun dot(other: ImmutableVec2f) = x * other.x + y * other.y

    override fun toString() = "($x, $y)"

    companion object {
        val UP      = ImmutableVec2f(0f, 1f)
        val DOWN    = ImmutableVec2f(0f, -1f)
        val RIGHT   = ImmutableVec2f(1f, 0f)
        val LEFT    = ImmutableVec2f(-1f, 0f)
    }
}
