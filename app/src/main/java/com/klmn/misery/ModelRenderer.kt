package com.klmn.misery

import com.klmn.misery.math.Camera
import com.klmn.misery.math.Transform

/**
 * ಠ^ಠ.
 * Created by Michael on 12/22/2018.
 */
data class Model(val mesh: Mesh, val texture: Texture)

/*
 Ideally, no Systems are implemented by the engine.
 The engine should make tasks like rendering & physics really simple to implement, and the game will implement the systems
 very easily. This should make the inheritance syntax redundant, and only the lambda approach will be available
*/
class ModelRenderer(
        private val shader: Shader,         // external values like these should be part of the game class, not the systems
        private val projection: Projection,
        private val camera: Camera
) : System(Model::class, Transform::class)
{
    override fun update(delta: Float, entity: Entity) {
        val (mesh, texture) = entity[Model::class]!!
        val transform = entity[Transform::class]!!

        shader.bind()
        shader.loadUniform("mvp", projection.asMatrix() * (camera.matrix * transform.matrix))
        shader.loadUniform("model", transform.matrix)
        texture.bind(1)
        shader.loadUniform("diffuse", 1)

        mesh.draw()

        texture.unbind()
        shader.unbind()
    }
}
