package com.klmn.misery.update

import com.klmn.misery.jni.MiseryJNI
import com.klmn.misery.render.Material
import com.klmn.misery.render.Mesh
import kotlin.reflect.KClass
import kotlin.reflect.full.safeCast

open class Entity {
    val id: Int = MiseryJNI.createEntity(this)

    operator fun <T : Any> get(type: String, cls: KClass<T>): T? = cls.safeCast(when (cls) {
        Motion::class -> Motion(MiseryJNI.getComponentPointer(id, type))
        Transform::class -> Transform(MiseryJNI.getComponentPointer(id, type))
        AABB::class -> AABB(MiseryJNI.getComponentPointer(id, type))
        Material::class -> Material(MiseryJNI.getMaterialComponent(id))
        Long::class -> MiseryJNI.getComponentPointer(id, type)
        else -> MiseryJNI.getComponent(id, type)
    })
    operator fun set(type: String, value: Any) = when(value::class) {
        Int::class -> MiseryJNI.putIntComponent(id, type, value as Int)
        Long::class -> MiseryJNI.putLongComponent(id, type, value as Long)
        Transform::class -> MiseryJNI.putTransformComponent(id, (value as Transform).toFloatArray())
        AABB::class -> MiseryJNI.putAABBComponent(id, (value as AABB).toFloatArray())
        Motion::class -> MiseryJNI.putMotionComponent(id, (value as Motion).toFloatArray())
        Material::class -> MiseryJNI.putMaterialComponent(id, (value as Material).data.id)
        Mesh::class -> MiseryJNI.putLongComponent(id, type, (value as Mesh).pointer)
        else -> MiseryJNI.putComponent(id, type, value);
    }

    fun destroy() = MiseryJNI.removeEntity(id)

    override fun equals(other: Any?) = other is Entity && other.id == id
    override fun hashCode() = id
}

fun entity(vararg pairs: Pair<String, Any>) = Entity().also {
    for ((key, value) in pairs) it[key] = value
}
fun entity(map: Map<String, Any>, vararg pairs: Pair<String, Any>) = Entity().also {
    for ((key, value) in map) it[key] = value
    for ((key, value) in pairs) it[key] = value
}
fun entity(vararg pairs: Pair<String, Any>, map: Map<String, Any>) = Entity().also {
    for ((key, value) in map) it[key] = value
    for ((key, value) in pairs) it[key] = value
}

fun system(vararg types: String, apply: (Entity, Float) -> Unit) =
        MiseryJNI.addSystem(arrayOf(*types), apply)

fun interaction(activeTypes: Array<String>, passiveTypes: Array<String>,
                apply: (Entity, Entity, Float) -> Unit) =
        MiseryJNI.addInteraction(activeTypes, passiveTypes, apply)
