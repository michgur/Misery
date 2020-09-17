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
class Texture
{
    var pointer: Long = 0L

    constructor(pointer: Long) { this.pointer = pointer }
    constructor(assets: AssetManager, path: String) : this(BitmapFactory.decodeStream(assets.open(path)))
    constructor(bitmap: Bitmap) {
        pointer = MiseryJNI.loadTexture(bitmap)
        bitmap.recycle()
    }

//    private var unit: Int = 0
//    fun bind(unit: Int = 0) {
//        this.unit = unit
//
//        glActiveTexture(GL_TEXTURE0 + unit)
//        glBindTexture(GL_TEXTURE_2D, id)
//    }
//    fun unbind() {
//        glActiveTexture(GL_TEXTURE0 + unit)
//        glBindTexture(GL_TEXTURE_2D, 0)
//    }
}
