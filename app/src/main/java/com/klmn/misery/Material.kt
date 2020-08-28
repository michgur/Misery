package com.klmn.misery

class Material(val data: Entity = Entity()) {
    var shader: Shader
        get() = Shader(data["shader", Int::class]!!)
        set(value) { data["shader"] = value.id }
    var diffuse: Texture
        get() = Texture(data["diffuse", Int::class]!!)
        set(value) { data["diffuse"] = value.id }
}