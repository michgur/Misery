package com.klmn.misery

import kotlin.reflect.KClass
import kotlin.reflect.full.safeCast

/**
 * ಠ^ಠ.
 * Created by Michael on 12/21/2018.
 */
class Entity(private val entityManager: EntityManager)
{
    private val components = mutableMapOf<KClass<*>, Any>()
    init { entityManager.addEntity(this) }

    fun containsAll(types: Collection<KClass<*>>) = components.keys.containsAll(types)
    operator fun contains(type: KClass<*>) = type in components
    fun <T : Any> put(value: T) = value::class.safeCast(components.put(value::class, value)).
            also { entityManager.entityChanged(this, value::class) }
    fun remove(type: KClass<*>) = components.remove(type).also { entityManager.entityChanged(this, type) }
    operator fun <T : Any> get(type: KClass<T>) = type.safeCast(components[type])
    operator fun <T : Any> set(type: KClass<T>, value: T) {
        components[type] = value
        entityManager.entityChanged(this, type)
    }
}

class Interaction(
        private val entityManager: EntityManager,
        private val active: Set<KClass<*>>,
        private val passive: Set<KClass<*>>,
        val execute: (Float, Entity, Entity) -> Unit
) {
    init {
        entityManager.addClass(active)
        entityManager.addClass(passive)
    }
    fun matches(active: Entity, passive: Entity) =
            active in entityManager.instancesOf(this.active) &&
            passive in entityManager.instancesOf(this.passive)
}

abstract class System(val game: Game, private val types: Set<KClass<*>>)
{
    constructor(game: Game, vararg types: KClass<*>) : this(game, types.toSet())
    init { game.entities.addClass(types) }

    open fun update(delta: Float) = entities.forEach { update(delta, it) }
    protected abstract fun update(delta: Float, entity: Entity)

    open val entities get() = game.entities.instancesOf(types)
}

// If this will eventually be the only way systems are created, [update] could be a lambda property, not a fun
fun system(game: Game, vararg types: KClass<*>, update: (Float, Entity) -> Unit = { _, _ -> }) =
    object : System(game, types.toSet()) { override fun update(delta: Float, entity: Entity) = update(delta, entity) }

private typealias EntityClass = Set<KClass<*>>
class EntityManager
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
