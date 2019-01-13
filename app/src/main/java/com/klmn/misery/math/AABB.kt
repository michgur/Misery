package com.klmn.misery.math

/**
 * ಠ^ಠ.
 * Created by Michael on 1/8/2019.
 */
data class AABB(var min: Vec3f, var max: Vec3f)
{
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

    infix fun intersects(aabb: AABB) = !(
            aabb.min.x > max.x || aabb.min.y > max.y || aabb.min.z > max.z ||
            aabb.max.x > min.x || aabb.max.y > min.y || aabb.max.z > min.z
    )

    operator fun contains(vec3f: Vec3f) =
        vec3f.x <= min.x && vec3f.y <= min.y && vec3f.z <= min.z &&
        vec3f.x <= max.x && vec3f.y <= max.y && vec3f.z <= max.z
    operator fun contains(aabb: AABB) = contains(aabb.min) && contains(aabb.max)
}
