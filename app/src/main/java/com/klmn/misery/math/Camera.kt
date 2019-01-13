package com.klmn.misery.math

/**
 * ಠ^ಠ.
 * Created by Michael on 12/28/2018.
 */
class Camera
{
    private val transform = Transform()

    var translation: Vec3f
        get() = transform.translation * -1f
        set(translation) { transform.translation = translation * -1f }
    var rotation: Quaternion
        get() = transform.rotation.conjugated
        set(rotation) { transform.rotation = rotation.conjugated
        }
    var scale: Vec3f
        get() = Vec3f(1f) / transform.scale
        set(scale) { transform.scale = Vec3f(1f) / scale }

    val matrix: Mat4f
        get() = transform.matrix
        //set() todo
}