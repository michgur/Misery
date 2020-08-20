package com.klmn.misery

import android.content.res.AssetManager
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.opengl.GLES30.*
import android.opengl.GLUtils


/**
 * ಠ^ಠ.
 * Created by Michael on 12/28/2018.
 */
class Texture
{
    val id: Int

    constructor(id: Int) { this.id = id }
    constructor(assets: AssetManager, path: String) : this(BitmapFactory.decodeStream(assets.open(path)))
    constructor(bitmap: Bitmap) {
        val intArray = IntArray(1)
        glGenTextures(1, intArray, 0)
        id = intArray[0]

        bind()
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR)
        GLUtils.texImage2D(GL_TEXTURE_2D, 0, bitmap, 0)
        unbind()

        bitmap.recycle()
    }

    private var unit: Int = 0
    fun bind(unit: Int = 0) {
        this.unit = unit

        glActiveTexture(GL_TEXTURE0 + unit)
        glBindTexture(GL_TEXTURE_2D, id)
    }
    fun unbind() {
        glActiveTexture(GL_TEXTURE0 + unit)
        glBindTexture(GL_TEXTURE_2D, 0)
    }
}
