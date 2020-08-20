package com.klmn.misery.math

import kotlin.math.cos
import kotlin.math.sin
import kotlin.math.sqrt

/**
 * ಠ^ಠ.
 * Created by Michael on 12/14/2018.
 */
data class Quaternion(var x: Float, var y: Float, var z: Float, var w: Float)
{
    constructor() : this(0f, 0f, 0f, 1f)
    constructor(vector: Vec3f, scalar: Float) : this(vector.x, vector.y, vector.z, scalar)
    constructor(f: FloatArray) : this(f[1], f[2], f[3], f[0])

    inline var scalar
        get() = w
        set(f) { w = f }
    inline var vector
        get() = Vec3f(x, y, z)
        set(vec3f) { x = vec3f.x; y = vec3f.y; z = vec3f.z }

    inline val length get() = sqrt(x * x + y * y + z * z + w * w)
    inline val normalized get() = this / length
    inline val conjugated get() = Quaternion(-x, -y, -z, w)

    operator fun plus(other: Quaternion) = Quaternion(x + other.x, y + other.y, z + other.z, w + other.w)
    operator fun minus(other: Quaternion) = Quaternion(x - other.x, y - other.y, z - other.z, w - other.w)

    operator fun plus(f: Float) = Quaternion(x + f, y + f, z + f, w + f)
    operator fun minus(f: Float) = Quaternion(x - f, y - f, z - f, w - f)
    operator fun times(f: Float) = Quaternion(x * f, y * f, z * f, w * f)
    operator fun div(f: Float) = Quaternion(x / f, y / f, z / f, w / f)

    operator fun times(other: Quaternion) = Quaternion(
            x * other.w + w * other.x + y * other.z - z * other.y,
            y * other.w + w * other.y + z * other.x - x * other.z,
            z * other.w + w * other.z + x * other.y - y * other.x,
            w * other.w - x * other.x - y * other.y - z * other.z
    )

    infix fun dot(other: Quaternion) = x * other.x + y * other.y + z * other.z + w * other.w
    infix fun rotate(vec3f: Vec3f) = (this * Quaternion(vec3f, 0f) * conjugated).vector

    inline val up get() = rotate(Vec3f.UP)
    inline val down get() = rotate(Vec3f.DOWN)
    inline val right get() = rotate(Vec3f.RIGHT)
    inline val left get() = rotate(Vec3f.LEFT)
    inline val forward get() = rotate(Vec3f.FORWARD)
    inline val back get() = rotate(Vec3f.BACK)

    override fun toString() = "[($x, $y, $z), $w]"
    fun toFloatArray() = floatArrayOf(w, x, y, z)

    companion object {
        fun rotation(axis: Vec3f, angle: Float) = Quaternion(axis * sin(angle / 2f), cos(angle / 2))
        fun fromMatrix(mat4f: Mat4f): Quaternion {
            val trace = mat4f[0] + mat4f[5] + mat4f[10]
            if (trace > 0f) {
                val s = 0.5f / sqrt(trace + 1f)
                return Quaternion(Vec3f(mat4f[6] - mat4f[9], mat4f[8] - mat4f[2], mat4f[1] - mat4f[4]) * s,
                        0.25f / s).normalized
            }
            if (mat4f[0] > mat4f[5] && mat4f[0] > mat4f[10]) {
                val s = 2f * sqrt(1f + mat4f[0] - mat4f[5] - mat4f[10])
                return Quaternion(0.25f * s, (mat4f[4] + mat4f[1]) / s, (mat4f[8] + mat4f[2]) / s,
                        (mat4f[6] - mat4f[9]) / s).normalized
            }
            if (mat4f[5] > mat4f[10]) {
                val s = 2f * sqrt(1f + mat4f[5] - mat4f[0] - mat4f[6])
                return Quaternion((mat4f[4] + mat4f[1]) / s, 0.25f * s, (mat4f[9] + mat4f[6]) / s,
                        (mat4f[8] - mat4f[2]) / s).normalized
            }
            val s = 2 * sqrt(1 + mat4f[10] - mat4f[0] - mat4f[5])
            return Quaternion((mat4f[8] + mat4f[2]) / s, (mat4f[6] + mat4f[9]) / s, 0.25f * s,
                    (mat4f[1] - mat4f[4]) / s).normalized
        }
    }
}
