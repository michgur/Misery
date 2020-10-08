package com.klmn.misery.render

import android.content.res.AssetManager
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.opengl.GLES30.*
import android.opengl.GLUtils
import com.klmn.misery.jni.MiseryJNI


/**
 * ಠ^ಠ.
 * Created by Michael on 12/28/2018.
 */
class Texture(val pointer: Long)
{
    constructor(assets: AssetManager, path: String) : this(BitmapFactory.decodeStream(assets.open(path)))
    constructor(bitmap: Bitmap) : this(MiseryJNI.loadTexture(bitmap)) {
        bitmap.recycle()
    }
}
