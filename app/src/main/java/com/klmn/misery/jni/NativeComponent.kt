package com.klmn.misery.jni

import com.klmn.misery.math.*
import kotlin.reflect.KProperty

class NativeFloatDelegate(val pointer: Long, val index: Int) {
    var field = if (pointer == 0L) 0f else null
    operator fun getValue(thisRef: Any?, property: KProperty<*>) =
            if (pointer == 0L) field!! else MiseryJNI.getFloat(pointer, index)
    operator fun setValue(thisRef: Any?, property: KProperty<*>, value: Float) {
        if (pointer == 0L) field = value
        else MiseryJNI.setFloat(pointer, value, index)
    }
}
class NativeVec2fDelegate(val pointer: Long, val index: Int) {
    var field: Vec2f? = if (pointer == 0L) Vec2f(0f) else null
    operator fun getValue(thisRef: Any?, property: KProperty<*>) =
            if (pointer == 0L) field!! else NativeVec2f(pointer, index)
    operator fun setValue(thisRef: Any?, property: KProperty<*>, value: Vec2f) {
        if (pointer == 0L) field = value
        else MiseryJNI.setFloats(pointer, value.toFloatArray(), index)
    }
}
class NativeVec3fDelegate(val pointer: Long, val index: Int) {
    var field: Vec3f? = if (pointer == 0L) Vec3f(0f) else null
    operator fun getValue(thisRef: Any?, property: KProperty<*>) =
            if (pointer == 0L) field!! else NativeVec3f(pointer, index)
    operator fun setValue(thisRef: Any?, property: KProperty<*>, value: Vec3f) {
        if (pointer == 0L) field = value
        else MiseryJNI.setFloats(pointer, value.toFloatArray(), index)
    }
}
class NativeQuaternionDelegate(val pointer: Long, val index: Int) {
    var field: Quaternion? = if (pointer == 0L) Quaternion() else null
    operator fun getValue(thisRef: Any?, property: KProperty<*>) =
        if (pointer == 0L) field!! else NativeQuaternion(pointer, index)
    operator fun setValue(thisRef: Any?, property: KProperty<*>, value: Quaternion) {
        if (pointer == 0L) field = value
        else MiseryJNI.setFloats(pointer, value.toFloatArray(), index)
    }
}

open class NativeComponent(var pointer: Long = 0) {
    inline val native get() = pointer != 0L
}
