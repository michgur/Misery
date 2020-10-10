package com.klmn.misery.math

import kotlin.math.tan

/**
 * ಠ^ಠ.
 * Created by Michael on 12/14/2018.
 */
class Mat4f
{
    private var m: FloatArray

    constructor() {
        m = floatArrayOf(
            1f, 0f, 0f, 0f,
            0f, 1f, 0f, 0f,
            0f, 0f, 1f, 0f,
            0f, 0f, 0f, 1f
        )
    }
    private constructor(m: FloatArray) { this.m = m }

    /* Access items by indices in the array */
    operator fun get(index: Int) = m[index]
    operator fun set(index: Int, f: Float) { m[index] = f }

    /* Access items by row & column */
    operator fun get(column: Int, row: Int) = m[column + row * 4]
    operator fun set(column: Int, row: Int, f: Float) { m[column + row * 4] = f }

    fun toFloatArray() = m.clone()

    fun transposed() = Mat4f(floatArrayOf(
            m[M00], m[M10], m[M20], m[M30],
            m[M01], m[M11], m[M21], m[M31],
            m[M02], m[M12], m[M22], m[M32],
            m[M03], m[M13], m[M23], m[M33]
    ))

    operator fun times(other: Mat4f) = Mat4f(floatArrayOf(
            m[M00] * other.m[M00] + m[M01] * other.m[M10] + m[M02] * other.m[M20] + m[M03] * other.m[M30],
            m[M00] * other.m[M01] + m[M01] * other.m[M11] + m[M02] * other.m[M21] + m[M03] * other.m[M31],
            m[M00] * other.m[M02] + m[M01] * other.m[M12] + m[M02] * other.m[M22] + m[M03] * other.m[M32],
            m[M00] * other.m[M03] + m[M01] * other.m[M13] + m[M02] * other.m[M23] + m[M03] * other.m[M33],
            m[M10] * other.m[M00] + m[M11] * other.m[M10] + m[M12] * other.m[M20] + m[M13] * other.m[M30],
            m[M10] * other.m[M01] + m[M11] * other.m[M11] + m[M12] * other.m[M21] + m[M13] * other.m[M31],
            m[M10] * other.m[M02] + m[M11] * other.m[M12] + m[M12] * other.m[M22] + m[M13] * other.m[M32],
            m[M10] * other.m[M03] + m[M11] * other.m[M13] + m[M12] * other.m[M23] + m[M13] * other.m[M33],
            m[M20] * other.m[M00] + m[M21] * other.m[M10] + m[M22] * other.m[M20] + m[M23] * other.m[M30],
            m[M20] * other.m[M01] + m[M21] * other.m[M11] + m[M22] * other.m[M21] + m[M23] * other.m[M31],
            m[M20] * other.m[M02] + m[M21] * other.m[M12] + m[M22] * other.m[M22] + m[M23] * other.m[M32],
            m[M20] * other.m[M03] + m[M21] * other.m[M13] + m[M22] * other.m[M23] + m[M23] * other.m[M33],
            m[M30] * other.m[M00] + m[M31] * other.m[M10] + m[M32] * other.m[M20] + m[M33] * other.m[M30],
            m[M30] * other.m[M01] + m[M31] * other.m[M11] + m[M32] * other.m[M21] + m[M33] * other.m[M31],
            m[M30] * other.m[M02] + m[M31] * other.m[M12] + m[M32] * other.m[M22] + m[M33] * other.m[M32],
            m[M30] * other.m[M03] + m[M31] * other.m[M13] + m[M32] * other.m[M23] + m[M33] * other.m[M33]
    ))
    operator fun times(vec3f: Vec3f) = Vec3f(
            m[M00] * vec3f.x + m[M01] * vec3f.y + m[M02] * vec3f.z + m[M03],
            m[M10] * vec3f.x + m[M11] * vec3f.y + m[M12] * vec3f.z + m[M13],
            m[M20] * vec3f.x + m[M21] * vec3f.y + m[M22] * vec3f.z + m[M23]
    )

    fun copy() = Mat4f(m.clone())
    override operator fun equals(other: Any?) = other is Mat4f && m.contentEquals(other.m)
    override fun hashCode() = m.hashCode()

    companion object {
        private const val M00 = 0; private const val M01 = 1; private const val M02 = 2; private const val M03 = 3;
        private const val M10 = 4; private const val M11 = 5; private const val M12 = 6; private const val M13 = 7;
        private const val M20 = 8; private const val M21 = 9; private const val M22 = 10; private const val M23 = 11;
        private const val M30 = 12; private const val M31 = 13; private const val M32 = 14; private const val M33 = 15;

        fun perspective(fov: Float, width: Float, height: Float, near: Float, far: Float): Mat4f {
            val mat4f = Mat4f()

            val ar = width / height
            val tan = tan(fov / 2f)
            val range = near - far

            mat4f.m[M00] = -1f / (tan * ar)
            mat4f.m[M11] = 1f / tan
            mat4f.m[M22] = (near + far) / range
            mat4f.m[M23] = 2f * far * near / range
            mat4f.m[M32] = -1f
            mat4f.m[M33] = 0f

            return mat4f
        }
    }

    override fun toString() =
        "${m[M00]}\t\t${m[M01]}\t\t${m[M02]}\t\t${m[M03]}\n" +
        "${m[M10]}\t\t${m[M11]}\t\t${m[M12]}\t\t${m[M13]}\n" +
        "${m[M20]}\t\t${m[M21]}\t\t${m[M22]}\t\t${m[M23]}\n" +
        "${m[M30]}\t\t${m[M31]}\t\t${m[M32]}\t\t${m[M33]}\n"
}