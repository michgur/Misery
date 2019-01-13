package com.klmn.misery

import kotlin.reflect.KClass
import kotlin.reflect.full.safeCast

/**
 * ಠ^ಠ.
 * Created by Michael on 12/21/2018.
 */
class Entity
{
    private val components = mutableMapOf<KClass<*>, Any>()
    init { EntityManager.addEntity(this) }

    fun containsAll(types: Collection<KClass<*>>) = components.keys.containsAll(types)
    operator fun contains(type: KClass<*>) = type in components
    fun <T : Any> put(value: T) = value::class.safeCast(components.put(value::class, value)).
            also { EntityManager.entityChanged(this, value::class) }
    fun remove(type: KClass<*>) = components.remove(type).also { EntityManager.entityChanged(this, type) }
    operator fun <T : Any> get(type: KClass<T>) = type.safeCast(components[type])
    operator fun <T : Any> set(type: KClass<T>, value: T) {
        components[type] = value
        EntityManager.entityChanged(this, type)
    }
}

class Interaction(
        private val active: Set<KClass<*>>,
        private val passive: Set<KClass<*>>,
        val execute: (Float, Entity, Entity) -> Unit
) {
    init {
        EntityManager.addClass(active)
        EntityManager.addClass(passive)
    }
    fun matches(active: Entity, passive: Entity) =
            active in EntityManager.instancesOf(this.active) &&
            passive in EntityManager.instancesOf(this.passive)
}

abstract class System(private val types: Set<KClass<*>>)
{
    constructor(vararg types: KClass<*>) : this(types.toSet())
    init { EntityManager.addClass(types) }

    open fun update(delta: Float) = entities.forEach { update(delta, it) }
    protected abstract fun update(delta: Float, entity: Entity)

    open val entities get() = EntityManager.instancesOf(types)
}

// If this will eventually be the only way systems are created, [update] could be a lambda property, not a fun
fun system(vararg types: KClass<*>, update: (Float, Entity) -> Unit = { _, _ -> }) =
    object : System(types.toSet()) { override fun update(delta: Float, entity: Entity) = update(delta, entity) }

private typealias EntityClass = Set<KClass<*>>
private object EntityManager
{
    private val entities = mutableSetOf<Entity>()
    private val classes = mutableMapOf<EntityClass, MutableSet<Entity>>()

    internal fun addClass(entityClass: EntityClass): MutableSet<Entity> {
        classes[entityClass] = entities.filter { it.containsAll(entityClass) }.toMutableSet()
        return classes[entityClass]!!
    }
    internal fun instancesOf(entityClass: EntityClass) = classes[entityClass] ?: addClass(entityClass)

    internal fun addEntity(entity: Entity) {
        entities.add(entity)
        classes.forEach { entityClass, instances ->
            if (entity.containsAll(entityClass)) instances.add(entity)
        }
    }
    internal fun entityChanged(entity: Entity, type: KClass<*>) {
        for ((entityClass, instances) in classes) {
            if (type !in entityClass) continue
            if (entity.containsAll(entityClass)) instances.add(entity)
        }
    }
}
