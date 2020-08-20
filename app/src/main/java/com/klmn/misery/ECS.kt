package com.klmn.misery

import kotlin.reflect.KClass
import kotlin.reflect.full.safeCast

class Entity {
    val id: Int = MiseryJNI.createEntity(this)

    operator fun <T : Any> get(type: String, cls: KClass<T>): T? = when (type) {
        "transform" -> cls.safeCast(Transform(MiseryJNI.getTransformComponent(id)))
        else -> cls.safeCast(MiseryJNI.getComponent(id, type))
    }
    operator fun set(type: String, value: Any) = when(type) {
        "transform" -> {
            require(value is Transform)
            MiseryJNI.setTransformComponent(id, value.toFloatArray())
        }
        else -> MiseryJNI.putComponent(id, type, value)
    }
}

fun system(vararg types: String, apply: (Entity, Float) -> Unit) { MiseryJNI.addSystem(arrayOf(*types), apply) }
