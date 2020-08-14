package com.klmn.misery

import android.app.Activity
import android.view.MotionEvent
import com.klmn.misery.math.*
import kotlin.math.max
import kotlin.math.min
import kotlin.random.Random
import kotlin.reflect.KClass

/**
 * ಠ^ಠ.
 * Created by Michael on 1/18/2019.
 */
open class Game(val activity: Activity)
{
    val entities = EntityManager()
    val renderSystems = mutableSetOf<System>()
    val updateSystems = mutableSetOf<System>()
    val inputBuffer = mutableSetOf<MotionEvent>()

    // will be part of a game loop or smth
    fun render(frameTime: Float) = renderSystems.forEach { it.update(frameTime) }
    fun update(delta: Float) {
        updateSystems.forEach { it.update(delta) }
        inputBuffer.clear()
    }

    open fun init() {}
    open fun onViewChanged(width: Int, height: Int) {}

    fun createEntity() = Entity(entities)
    fun createRenderSystem(vararg types: KClass<*>, update: (Float, Entity) -> Unit) = createRenderSystem(types.toSet(), update)
    fun createRenderSystem(types: Set<KClass<*>>, update: (Float, Entity) -> Unit)
            = renderSystems.add(object : System(this, types) {
        override fun update(delta: Float, entity: Entity) = update(delta, entity)
    })
    fun createUpdateSystem(vararg types: KClass<*>, update: (Float, Entity) -> Unit) = createUpdateSystem(types.toSet(), update)
    fun createUpdateSystem(types: Set<KClass<*>>, update: (Float, Entity) -> Unit)
            = updateSystems.add(object : System(this, types) {
        override fun update(delta: Float, entity: Entity) = update(delta, entity)
    })
}

class PigGame(activity: Activity) : Game(activity)
{
    private lateinit var projection: Projection
    override fun onViewChanged(width: Int, height: Int) {
        projection = Perspective(Math.PI.toFloat() * 0.5f, width, height, 0.1f, 1000f)
    }

    override fun init() {
        val shader = Shader("vertex.glsl", "fragment.glsl")
        val camera = Camera()

        val mesh = Mesh("pig.obj")
        val texture = Texture(activity.assets, "pig.png")
        val transform = Transform(
                scale = Vec3f(0.1f),
                translation = Vec3f(0f, -1f, 20f),
                rotation = Quaternion.rotation(Vec3f.LEFT, 0.5f)
        )
        for (i in 0..5) {
            val pig = createEntity()
            pig.put(Model(mesh, texture))
            pig.put(transform.copy(scale = Vec3f(0.05f), translation = Vec3f(
                    Random.nextFloat() * 10f - 5f,
                    Random.nextFloat() * 10f - 5f,
                    Random.nextFloat() * 30f + 10f
            )))
            var touchID = -1
            var touchPos: Vec2f? = null
            val movementControl = TouchControls(
                    MotionEvent.ACTION_DOWN to { event ->
                        touchPos = Vec2f(event.x, event.y)
                        touchID = event.getPointerId(0)
                    },
                    MotionEvent.ACTION_MOVE to { event ->
                        if (touchPos != null && event.findPointerIndex(touchID) >= 0) {
                            val newPos = Vec2f(
                                    event.getX(event.findPointerIndex(touchID)),
                                    event.getY(event.findPointerIndex(touchID))
                            )
                            val pigTransform = this[Transform::class]!!
                            pigTransform.translation += Vec3f(
                                    (touchPos!!.x - newPos.x) * -0.02f,
                                    0f,
                                    (touchPos!!.y - newPos.y) * 0.01f
                            )
                            fun clamp(value: Float, bottom: Float, top: Float) = min(top, max(bottom, value))
                            pigTransform.translation =
                                    Vec3f(
                                            clamp(pigTransform.translation.x, -15f, 15f),
                                            pigTransform.translation.y,
                                            clamp(pigTransform.translation.z, -2f, 40f)
                                    )

                            touchPos = newPos
                        }
                    }
            )
            pig.put(movementControl)
        }

        createRenderSystem(Model::class, Transform::class) { _, entity ->
            renderModel(entity[Model::class]!!, entity[Transform::class]!!, shader, projection, camera)
        }

        val collidable = setOf(Transform::class, AABB::class)
        val interactionSystem = InteractionSystem(this, collidable)
        updateSystems.add(interactionSystem)
        interactionSystem.addInteraction(Interaction(entities, collidable, collidable) { delta, a, b ->
            println("$a intersects $b (delta $delta)")
        })

        val velocity = 0.1f
        createUpdateSystem(Transform::class) { delta, entity ->
            entity[Transform::class]!!.rotation *= Quaternion.rotation(Vec3f.UP, delta * velocity * Math.PI.toFloat())
        }

        createUpdateSystem(TouchControls::class) { _, entity ->
            inputBuffer.forEach { entity[TouchControls::class]!!.onTouchEvent(entity, it) }
        }
    }
}
