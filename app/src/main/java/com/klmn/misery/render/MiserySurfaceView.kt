package com.klmn.misery.render

import android.content.Context
import android.view.SurfaceHolder
import android.view.SurfaceView
import com.klmn.misery.jni.MiseryJNI

/* TODO:
    -figure out input management, preferably where functions are not stored as a component but as a system
    -native code- urgent organization, try avoiding global variables (have create and set methods
    to avoid increased JNI traffic)
    -debug mesh loading code- some serious bugginess there
    -give Game class some actual options
    -a proper render & physics engines
    -turn this into a library and export
 */

class MiserySurfaceView(context: Context) : SurfaceView(context), SurfaceHolder.Callback {
    init { holder.addCallback(this) }

    override fun surfaceCreated(holder: SurfaceHolder) {}

    override fun surfaceChanged(holder: SurfaceHolder, format: Int, width: Int, height: Int) =
            MiseryJNI.setSurface(holder.surface, context.assets)

    override fun surfaceDestroyed(holder: SurfaceHolder) = MiseryJNI.releaseSurface()
}