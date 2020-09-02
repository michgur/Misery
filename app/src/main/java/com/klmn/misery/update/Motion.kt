package com.klmn.misery.update

import com.klmn.misery.jni.MiseryJNI
import com.klmn.misery.jni.NativeComponent
import com.klmn.misery.jni.NativeVec3fDelegate
import com.klmn.misery.math.Vec3f
import kotlin.math.pow

/**
 * ಠ^ಠ.
 * Created by Michael on 1/11/2019.
 */
class Motion : NativeComponent {
    constructor(pointer: Long) :super(pointer)
    constructor(velocity: Vec3f = Vec3f(), acceleration: Vec3f = Vec3f()) {
        this.velocity = velocity
        this.acceleration = acceleration
    }

    var velocity: Vec3f by NativeVec3fDelegate(3)
    var acceleration: Vec3f by NativeVec3fDelegate(0)

    fun toFloatArray() =
            if (native) MiseryJNI.getFloats(pointer, 6, 0)
            else floatArrayOf(*velocity.toFloatArray(), *acceleration.toFloatArray())

    companion object {
        private val coefficient = 1f / (2f - 2f.pow(1f/3f))
        private val complement = 1f - 2f * coefficient
        private fun verlet(pos: Vec3f, motion: Motion, delta: Float): Vec3f {
            var res = pos
            val halfDelta = delta / 2f
            res += motion.velocity * halfDelta
            motion.velocity += motion.acceleration * delta
            return res + motion.velocity * halfDelta
        }

        fun forestRuth(pos: Vec3f, motion: Motion, delta: Float): Vec3f {
            var res = verlet(pos, motion, delta * coefficient);
            res = verlet(res, motion, delta * complement);
            return verlet(res, motion, delta * coefficient);
        }
    }
}