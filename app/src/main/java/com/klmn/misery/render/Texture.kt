package com.klmn.misery.render

import com.klmn.misery.jni.MiseryJNI


/**
 * ಠ^ಠ.
 * Created by Michael on 12/28/2018.
 */
class Texture(val pointer: Long) {
    constructor(path: String) : this(MiseryJNI.loadTexture(path))
}
