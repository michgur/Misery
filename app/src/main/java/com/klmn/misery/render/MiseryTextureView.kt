package com.klmn.misery.render

import android.content.Context
import android.graphics.SurfaceTexture
import android.view.TextureView
import javax.microedition.khronos.egl.EGLContext

class MiseryTextureView(context: Context) : TextureView(context), TextureView.SurfaceTextureListener {
    override fun onSurfaceTextureAvailable(surface: SurfaceTexture, width: Int, height: Int) {
        // start game (only render?) thread
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