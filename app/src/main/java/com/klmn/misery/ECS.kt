package com.klmn.misery

import kotlin.reflect.KClass
import kotlin.reflect.full.safeCast

class Entity {
    val id: Int = MiseryJNI.createEntity(this)

    operator fun <T : Any> get(type: String, cls: KClass<T>): T? = when (type) {
        "transform" -> Transform(MiseryJNI.getTransformComponent(id)) as T?
        "material" -> Material(MiseryJNI.getMaterialComponent(id)) as T?
        else -> cls.safeCast(MiseryJNI.getComponent(id, type))
    }
    operator fun set(type: String, value: Any) = when(value::class) {
        Int::class -> MiseryJNI.setIntComponent(id, type, value as Int)
        Long::class -> MiseryJNI.setLongComponent(id, type, value as Long)
        Transform::class -> MiseryJNI.setTransformComponent(id, (value as Transform).toFloatArray())
        Material::class -> MiseryJNI.setMaterialComponent(id, (value as Material).data.id)
        Mesh::class -> MiseryJNI.setLongComponent(id, type, (value as Mesh).pointer)
        else -> MiseryJNI.putComponent(id, type, value);
    }
}

fun system(vararg types: String, apply: (Entity, Float) -> Unit) { MiseryJNI.addSystem(arrayOf(*types), apply) }
