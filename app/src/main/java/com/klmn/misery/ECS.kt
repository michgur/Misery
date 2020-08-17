package com.klmn.misery

import kotlin.reflect.KClass
import kotlin.reflect.safeCast

class Entity(val id: Int = MiseryJNI.createEntity()) {
    @OptIn(ExperimentalStdlibApi::class)
    operator fun <T: Any> get(type: String, cls: KClass<T>): T? { return cls.safeCast(MiseryJNI.getComponent(id, type)) }
    operator fun set(type: String, value: Any) { MiseryJNI.putComponent(id, type, value) }
}

fun system(vararg types: String, apply: (Entity, Float) -> Unit) { MiseryJNI.addSystem(arrayOf(*types), apply) }
