package com.klmn.misery

import com.klmn.misery.math.Camera
import com.klmn.misery.math.Transform

/**
 * ಠ^ಠ.
 * Created by Michael on 12/22/2018.
 */
data class Model(val mesh: Mesh, val texture: Texture)

fun renderModel(model: Model, transform: Transform, shader: Shader, projection: Projection, camera: Camera) {
    shader.bind()
    shader.loadUniform("mvp", projection.asMatrix() * (camera.matrix * transform.matrix))
//    shader.loadUniform("model", transform.matrix)
    model.texture.bind(1)
    shader.loadUniform("diffuse", 1)

    model.mesh.draw()

    model.texture.unbind()
    shader.unbind()
}

