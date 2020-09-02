package com.klmn.misery.jni

import com.klmn.misery.math.Quaternion
import com.klmn.misery.math.Vec3f
import kotlin.reflect.KProperty

class NativeVec3fDelegate(val offset: Int) {
    var value = Vec3f()
    operator fun getValue(thisRef: NativeComponent, property: KProperty<*>) =
        if (thisRef.native) Vec3f(MiseryJNI.getFloats(thisRef.pointer, 3, offset)) else value
    operator fun setValue(thisRef: NativeComponent, property: KProperty<*>, value: Vec3f) {
        if (thisRef.native) MiseryJNI.setFloats(thisRef.pointer, value.toFloatArray(), offset)
        else this.value = value
    }
}

class NativeQuaternionDelegate(val offset: Int) {
    var value = Quaternion()
    operator fun getValue(thisRef: NativeComponent, property: KProperty<*>) =
        if (thisRef.native) Quaternion(MiseryJNI.getFloats(thisRef.pointer, 4, offset)) else value
    operator fun setValue(thisRef: NativeComponent, property: KProperty<*>, value: Quaternion) {
        if (thisRef.native) MiseryJNI.setFloats(thisRef.pointer, value.toFloatArray(), offset)
        else this.value = value
    }
}

open class NativeComponent(var pointer: Long) {
    var native = true

    constructor() : this(0) { native = false }
}