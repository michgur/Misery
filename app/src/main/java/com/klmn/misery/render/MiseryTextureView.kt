package com.klmn.misery.render

import android.content.Context
import android.graphics.SurfaceTexture
import android.view.Surface
import android.view.TextureView
import com.klmn.misery.jni.MiseryJNI

class MiseryTextureView(context: Context) : TextureView(context), TextureView.SurfaceTextureListener {
    init { surfaceTextureListener = this }

    override fun onSurfaceTextureAvailable(surface: SurfaceTexture, width: Int, height: Int) {
        // start game (only render?) thread
        MiseryJNI.startThread(context.assets, Surface(surface))
    }

    override fun onSurfaceTextureSizeChanged(surface: SurfaceTexture, width: Int, height: Int) {
        // update view size
    }

    override fun onSurfaceTextureDestroyed(surface: SurfaceTexture): Boolean {
        // destroy thread
        return true
    }

    override fun onSurfaceTextureUpdated(surface: SurfaceTexture) {
        // no idea
    }

}