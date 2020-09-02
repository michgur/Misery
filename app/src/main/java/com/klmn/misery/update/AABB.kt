package com.klmn.misery.update

import com.klmn.misery.jni.MiseryJNI
import com.klmn.misery.jni.NativeComponent
import com.klmn.misery.jni.NativeVec3fDelegate
import com.klmn.misery.math.Mat4f
import com.klmn.misery.math.Quaternion
import com.klmn.misery.math.Vec3f

/**
 * ಠ^ಠ.
 * Created by Michael on 1/8/2019.
 */
class AABB : NativeComponent {
    constructor(pointer: Long) : super(pointer)
    constructor(min: Vec3f = Vec3f(), max: Vec3f = Vec3f()) {
        this.pointer = 0
        this.native = false
        this.min = min
        this.max = max
    }

    var min : Vec3f by NativeVec3fDelegate(0)
    var max : Vec3f by NativeVec3fDelegate(3)
//    var min = Vec3f()
//        get() = if (native) Vec3f(MiseryJNI.getFloats(pointer, 3, 0)) else field
//        set(value) = if (native) MiseryJNI.setFloats(pointer, value.toFloatArray(), 0) else field = value
//    var max = Vec3f()
//        get() = if (native) Vec3f(MiseryJNI.getFloats(pointer, 3, 3)) else field
//        set(value) = if (native) MiseryJNI.setFloats(pointer, value.toFloatArray(), 3) else field = value

    inline var center
        get() = (max + min) / 2f
        set(center) = extents.let {
            min = center - it
            max = center + it
        }
    inline var extents
        get() = (max - min) / 2f
        set(extents) = center.let {
            min = it - extents
            max = it + extents
        }
    inline var size
        get() = max - min
        set(size) { extents = size / 2f }

    fun transform(transform: Mat4f) = AABB(transform * min, transform * max)

    infix fun intersects(other: AABB) = !(min.x > other.max.x || max.x < other.min.x || min.y > other.max.y ||
            max.y < other.min.y || min.z > other.max.z || max.z < other.min.z)

    operator fun contains(vec3f: Vec3f) =
        vec3f.x <= min.x && vec3f.y <= min.y && vec3f.z <= min.z &&
        vec3f.x <= max.x && vec3f.y <= max.y && vec3f.z <= max.z
    operator fun contains(aabb: AABB) = contains(aabb.min) && contains(aabb.max)

    fun toFloatArray() =
            if (native) MiseryJNI.getFloats(pointer, 6, 0)
            else floatArrayOf(*min.toFloatArray(), *max.toFloatArray())
}
