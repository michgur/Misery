package com.klmn.misery.render

import android.opengl.GLES30.*
import com.klmn.misery.jni.MiseryJNI
import com.klmn.misery.math.Mat4f
import com.klmn.misery.math.Vec2f
import com.klmn.misery.math.Vec3f

/**
 * ಠ^ಠ.
 * Created by Michael on 12/22/2018.
 */
class Shader(val pointer: Long)
{
    constructor(vertex: String, fragment: String) : this(MiseryJNI.createProgram(vertex, fragment))
}
