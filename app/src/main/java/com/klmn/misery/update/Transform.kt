package com.klmn.misery.update

import com.klmn.misery.MiseryJNI
import com.klmn.misery.math.Mat4f
import com.klmn.misery.math.Quaternion
import com.klmn.misery.math.Vec3f

class Transform {
    private var native: Boolean
    var pointer: Long

    constructor(pointer: Long) {
        this.pointer = pointer
        this.native = true
    }
    constructor(translation: Vec3f = Vec3f(), rotation: Quaternion = Quaternion(), scale: Vec3f = Vec3f(1f)) {
        this.pointer = 0
        this.native = false
        this.translation = translation
        this.rotation = rotation
        this.scale = scale
    }

    var translation = Vec3f()
        get() = if (native) Vec3f(MiseryJNI.getFloats(pointer, 3, 0)) else field
        set(value) = if (native) MiseryJNI.setFloats(pointer, value.toFloatArray(), 0) else field = value
    var rotation = Quaternion()
        get() = if (native) Quaternion(MiseryJNI.getFloats(pointer, 4, 3)) else field
        set(value) = if (native) MiseryJNI.setFloats(pointer, value.toFloatArray(), 3) else field = value
    var scale = Vec3f(1f)
        get() = if (native) Vec3f(MiseryJNI.getFloats(pointer, 3, 7)) else field
        set(value) = if (native) MiseryJNI.setFloats(pointer, value.toFloatArray(), 7) else field = value

    fun toFloatArray() =
        if (native) MiseryJNI.getFloats(pointer, 10, 0)
        else floatArrayOf(*translation.toFloatArray(), *rotation.toFloatArray(), *scale.toFloatArray())

    var matrix: Mat4f
        get(): Mat4f {
            val m = Mat4f()

            val (translation, rotation, scale) = this
            val r2 = rotation * 2f
            val xx2 = rotation.x * r2.x;
            val yy2 = rotation.y * r2.y;
            val zz2 = rotation.z * r2.z;
            val xy2 = rotation.x * r2.y;
            val yz2 = rotation.y * r2.z;
            val xz2 = rotation.x * r2.z;
            val xw2 = rotation.w * r2.x;
            val yw2 = rotation.w * r2.y;
            val zw2 = rotation.w * r2.z;

            m[0] = (1f - (yy2 + zz2)) * scale.x
            m[1] = (xy2 - zw2) * scale.y
            m[2] = (xz2 + yw2) * scale.z
            m[3] = translation.x
            m[4] = (xy2 + zw2) * scale.x
            m[5] = (1f - (xx2 + zz2)) * scale.y
            m[6] = (yz2 - xw2) * scale.z
            m[7] = translation.y
            m[8] = (xz2 - yw2) * scale.x
            m[9] = (yz2 + xw2) * scale.y
            m[10] = (1f - (xx2 + yy2)) * scale.z
            m[11] = translation.z

            return m
        }
        set(matrix) {
            var c0 = Vec3f(matrix[0], matrix[4], matrix[8])
            var c1 = Vec3f(matrix[1], matrix[5], matrix[9])
            var c2 = Vec3f(matrix[2], matrix[6], matrix[10])
            translation = Vec3f(matrix[3], matrix[7], matrix[11])
            scale = Vec3f(c0.length, c1.length, c2.length)
            c0 /= scale.x; c1 /= scale.y; c2 /= scale.z
            val r = Mat4f()
            r[0] = c0.x; r[1] = c1.x; r[2] = c2.x
            r[4] = c0.y; r[5] = c1.y; r[6] = c2.y
            r[8] = c0.z; r[9] = c1.z; r[10] = c2.z
            rotation = Quaternion.fromMatrix(r)
        }

    override fun equals(other: Any?): Boolean {
        if (this === other) return true
        if (javaClass != other?.javaClass) return false

        other as Transform
        return if (native) pointer == other.pointer
        else translation == other.translation && rotation == other.rotation && scale == other.scale
    }

    override fun hashCode(): Int {
        if (native) return pointer.hashCode()
        var result = translation.hashCode()
        result = 31 * result + rotation.hashCode()
        result = 31 * result + scale.hashCode()
        return result
    }

    override fun toString(): String {
        return "Transform${if (native) "(native[${pointer}])" else ""}{\n" +
                "\ttranslation: $translation\n\trotation: $rotation\n\tscale: $scale\n}"
    }

    fun copy(translation: Vec3f = this.translation,
             rotation: Quaternion = this.rotation,
             scale: Vec3f = this.scale) = Transform(translation, rotation, scale)

    operator fun component1() = translation
    operator fun component2() = rotation
    operator fun component3() = scale
}