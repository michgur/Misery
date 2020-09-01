package com.klmn.misery.update

import com.klmn.misery.MiseryJNI
import com.klmn.misery.math.AABB
import com.klmn.misery.render.Material
import com.klmn.misery.render.Mesh
import kotlin.reflect.KClass
import kotlin.reflect.full.safeCast

open class Entity(vararg components: Pair<String, Any>) {
    val id: Int = MiseryJNI.createEntity(this)

    init { for (component in components) set(component.first, component.second) }

    operator fun <T : Any> get(type: String, cls: KClass<T>): T? = when (cls) {
        Transform::class -> Transform(MiseryJNI.getTransformComponent(id)) as T?
        Material::class -> Material(MiseryJNI.getMaterialComponent(id)) as T?
        Long::class -> MiseryJNI.getComponentPointer(id, type) as T?
        else -> cls.safeCast(MiseryJNI.getComponent(id, type))
    }
    operator fun set(type: String, value: Any) = when(value::class) {
        Int::class -> MiseryJNI.putIntComponent(id, type, value as Int)
        Long::class -> MiseryJNI.putLongComponent(id, type, value as Long)
        Transform::class -> MiseryJNI.setTransformComponent(id, (value as Transform).toFloatArray())
        AABB::class -> MiseryJNI.setAABBComponent(id, (value as AABB).toFloatArray())
        Material::class -> MiseryJNI.setMaterialComponent(id, (value as Material).data.id)
        Mesh::class -> MiseryJNI.putLongComponent(id, type, (value as Mesh).pointer)
        else -> MiseryJNI.putComponent(id, type, value);
    }

    fun destroy() = MiseryJNI.removeEntity(id)

    override fun equals(other: Any?) = other is Entity && other.id == id
    override fun hashCode() = id
}

fun system(vararg types: String, apply: (Entity, Float) -> Unit) =
        MiseryJNI.addSystem(arrayOf(*types), apply)

fun interaction(activeTypes: Array<String>, passiveTypes: Array<String>,
                apply: (Entity, Entity, Float) -> Unit) =
        MiseryJNI.addInteraction(activeTypes, passiveTypes, apply)
