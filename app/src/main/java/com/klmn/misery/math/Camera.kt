package com.klmn.misery.math

import com.klmn.misery.jni.MiseryJNI
import com.klmn.misery.update.Transform

/**
 * ಠ^ಠ.
 * Created by Michael on 12/28/2018.
 */
object Camera
{
    private val transform = Transform(MiseryJNI.getCameraTransform())

    var translation: Vec3f
        get() = transform.translation * -1f
        set(translation) { transform.translation = translation * -1f }
    var rotation: Quaternion
        get() = transform.rotation.conjugated
        set(rotation) { transform.rotation = rotation.conjugated }
    var scale: Vec3f
        get() = Vec3f(1f) / transform.scale
        set(scale) { transform.scale = Vec3f(1f) / scale }

    val matrix: Mat4f
        get() = transform.matrix
        //set() todo
}