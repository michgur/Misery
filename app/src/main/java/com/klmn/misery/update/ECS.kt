package com.klmn.misery.update

import com.klmn.misery.MiseryJNI
import com.klmn.misery.render.Material
import com.klmn.misery.render.Mesh
import kotlin.reflect.KClass
import kotlin.reflect.full.safeCast

open class Entity {
    val id: Int = MiseryJNI.createEntity(this)

    operator fun <T : Any> get(type: String, cls: KClass<T>): T? = when (type) {
        "transform" -> Transform(MiseryJNI.getTransformComponent(id)) as T?
        "material" -> Material(MiseryJNI.getMaterialComponent(id)) as T?
        else -> cls.safeCast(MiseryJNI.getComponent(id, type))
    }
    operator fun set(type: String, value: Any) = when(value::class) {
        Int::class -> MiseryJNI.putIntComponent(id, type, value as Int)
        Long::class -> MiseryJNI.putLongComponent(id, type, value as Long)
        Transform::class -> MiseryJNI.setTransformComponent(id, (value as Transform).toFloatArray())
        Material::class -> MiseryJNI.setMaterialComponent(id, (value as Material).data.id)
        Mesh::class -> MiseryJNI.putLongComponent(id, type, (value as Mesh).pointer)
        else -> MiseryJNI.putComponent(id, type, value);
    }

    fun destroy() = MiseryJNI.removeEntity(id)

    override fun equals(other: Any?) = other is Entity && other.id == id
    override fun hashCode() = id
}

fun system(vararg types: String, apply: (Entity, Float) -> Unit) {
    MiseryJNI.addSystem(arrayOf(*types), apply)
}
