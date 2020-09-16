package com.klmn.misery.jni

import com.klmn.misery.math.ImmutableQuaternion
import com.klmn.misery.math.ImmutableVec3f
import kotlin.reflect.KProperty

// todo: try implementing these via bytebuffers, could possibly reduce JNI traffic?
class NativeFloatDelegate(val offset: Int) {
    var value = 0f
    operator fun getValue(thisRef: NativeComponent, property: KProperty<*>) =
            if (thisRef.native) MiseryJNI.getFloat(thisRef.pointer, offset) else value
    operator fun setValue(thisRef: NativeComponent, property: KProperty<*>, value: Float) {
        if (thisRef.native) MiseryJNI.setFloat(thisRef.pointer, value, offset)
        else this.value = value
    }
}

class NativeVec3fDelegate(val offset: Int) {
    var value = ImmutableVec3f()
    operator fun getValue(thisRef: NativeComponent, property: KProperty<*>) =
        if (thisRef.native) ImmutableVec3f(MiseryJNI.getFloats(thisRef.pointer, 3, offset)) else value
    operator fun setValue(thisRef: NativeComponent, property: KProperty<*>, value: ImmutableVec3f) {
        if (thisRef.native) MiseryJNI.setFloats(thisRef.pointer, value.toFloatArray(), offset)
        else this.value = value
    }
}

class NativeQuaternionDelegate(val offset: Int) {
    var value = ImmutableQuaternion()
    operator fun getValue(thisRef: NativeComponent, property: KProperty<*>) =
        if (thisRef.native) ImmutableQuaternion(MiseryJNI.getFloats(thisRef.pointer, 4, offset)) else value
    operator fun setValue(thisRef: NativeComponent, property: KProperty<*>, value: ImmutableQuaternion) {
        if (thisRef.native) MiseryJNI.setFloats(thisRef.pointer, value.toFloatArray(), offset)
        else this.value = value
    }
}

open class NativeComponent(var pointer: Long) {
    var native = true

    constructor() : this(0) { native = false }
}
