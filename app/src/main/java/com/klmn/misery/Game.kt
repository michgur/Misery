package com.klmn.misery

import android.app.Activity
import android.opengl.GLES30.*
import android.view.MotionEvent
import com.klmn.misery.jni.MiseryJNI
import com.klmn.misery.math.*
import com.klmn.misery.render.Material
import com.klmn.misery.render.Mesh
import com.klmn.misery.render.Shader
import com.klmn.misery.render.Texture
import com.klmn.misery.update.*
import java.nio.FloatBuffer
import java.util.concurrent.locks.ReentrantLock
import kotlin.math.acos
import kotlin.math.pow
import kotlin.random.Random

/**
 * ಠ^ಠ.
 * Created by Michael on 1/18/2019.
 */
abstract class Game(val activity: Activity)
{
    val inputBuffer = mutableSetOf<MotionEvent>()
    val inputLock = ReentrantLock()

    abstract fun init()
}
