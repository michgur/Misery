package com.klmn.misery.update

import com.klmn.misery.math.AABB
import com.klmn.misery.math.Vec3f
import java.util.*
import kotlin.reflect.KClass

/**
 * ಠ^ಠ.
 * Created by Michael on 1/8/2019.
 */
// todo: think how to abstract this class (could be part of ECS.kt if necessary)
//class InteractionSystem(game: Game, types: Set<KClass<*>>) : System(game, types)
//{
//    constructor(game: Game, vararg types: KClass<*>) : this(game, types.toSet())
//
//    private val interactions = mutableSetOf<Interaction>()
//    fun addInteraction(interaction: Interaction) = interactions.add(interaction)
//
//    private var sum = Vec3f()
//    private var sqrSum = Vec3f()
//    private var axis = 0
//
//    override val entities = sortedSetOf<Entity>(Comparator(::compare))
//
//    fun compare(a: Entity, b: Entity) = a[AABB::class]!!.min[axis].compareTo(b[AABB::class]!!.min[axis])
//    fun postUpdate() {
//        val avg = sum / entities.size.toFloat()
//        val variance = sqrSum - (sum * avg)
//        axis = (0..2).maxBy { variance[it] }!!
//        sum = Vec3f()
//        sqrSum = Vec3f()
//    }
//
//    override fun update(delta: Float) {
//        super.update(delta)
//        postUpdate()
//    }
//
//    override fun update(delta: Float, entity: Entity) {
//        val aabb = entity[AABB::class]!!
//        val center = aabb.center
//        sum += center
//        sqrSum += center * center
//        for (other in entities) {
//            val otherAABB = other[AABB::class]!!
//            if (aabb.max[axis] < otherAABB.min[axis]) break
//            if (aabb intersects otherAABB)
//                for (interaction in interactions)
//                    if (interaction.matches(entity, other)) {
//                        interaction.execute(delta, entity, other)
//                        break
//                    }
//        }
//    }
//}
