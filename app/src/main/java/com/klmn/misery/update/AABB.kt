package com.klmn.misery.update

import com.klmn.misery.jni.MiseryJNI
import com.klmn.misery.jni.NativeComponent
import com.klmn.misery.jni.NativeVec3fDelegate
import com.klmn.misery.math.*

/**
 * ಠ^ಠ.
 * Created by Michael on 1/8/2019.
 */
class AABB : NativeComponent {
    constructor(pointer: Long) : super(pointer)
    constructor(min: Vec3f = Vec3f(-1f), max: Vec3f = Vec3f(1f)) {
        this.min = min
        this.max = max
    }

    var min : Vec3f by NativeVec3fDelegate(pointer, 0)
    var max : Vec3f by NativeVec3fDelegate(pointer, 3)

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
