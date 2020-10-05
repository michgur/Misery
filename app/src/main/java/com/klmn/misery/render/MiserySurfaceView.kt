package com.klmn.misery.render

import android.content.Context
import android.view.SurfaceHolder
import android.view.SurfaceView
import com.klmn.misery.jni.MiseryJNI


class MiserySurfaceView(context: Context) : SurfaceView(context), SurfaceHolder.Callback {
    init { holder.addCallback(this) }

    override fun surfaceCreated(holder: SurfaceHolder) {}

    override fun surfaceChanged(holder: SurfaceHolder, format: Int, width: Int, height: Int) =
            MiseryJNI.initEGL(holder.surface, context.assets)

    override fun surfaceDestroyed(holder: SurfaceHolder) {}
}