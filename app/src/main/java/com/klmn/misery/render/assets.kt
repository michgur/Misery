package com.klmn.misery.render

import com.klmn.misery.jni.MiseryJNI

/**
 * ಠ^ಠ.
 * Created by Michael on 12/22/2018.
 */
class Mesh(val pointer: Long) {
    constructor(path: String) : this(MiseryJNI.loadMesh(path))
}

class Shader(val pointer: Long) {
    constructor(vertex: String, fragment: String) : this(MiseryJNI.createProgram(vertex, fragment))
}

class Texture(val pointer: Long) {
    constructor(path: String) : this(MiseryJNI.loadTexture(path))
}
