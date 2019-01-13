package com.klmn.misery

import com.klmn.misery.math.Mat4f
import kotlin.math.tan

/**
 * ಠ^ಠ.
 * Created by Michael on 12/23/2018.
 */
interface Projection { fun asMatrix(): Mat4f }

data class Perspective(
        val fov: Float,
        val width: Int,
        val height: Int,
        val near: Float,
        val far: Float
) : Projection {
    private val m = Mat4f()
    init {
        val rtan = 1f / tan(fov / 2f)
        val zScale = if (near == far) 1f else far / (far - near)
        val zOffset = -near * zScale

        m[0] = rtan
        m[5] = width.toFloat() / height * rtan
        m[10] = zScale
        m[11] = zOffset
        m[14] = 1f
        m[15] = 0f
    }

    override fun asMatrix() = m
}
