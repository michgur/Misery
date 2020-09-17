package com.klmn.misery.render

import com.klmn.misery.update.Entity

class Material(val data: Entity = Entity()) {
    constructor(shader: Shader, diffuse: Texture = Texture(0)) : this() {
        this.shader = shader
        this.diffuse = diffuse
    }

    var shader: Shader
        get() = Shader(data["shader", Int::class]!!)
        set(value) { data["shader"] = value.id }
    var diffuse: Texture
        get() = Texture(data["diffuse", Long::class]!!)
        set(value) { data["diffuse"] = value.pointer }
}