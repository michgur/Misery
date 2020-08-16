package com.klmn.misery

import kotlin.reflect.KClass
import kotlin.reflect.safeCast

class EntityJNI {
    val id = MiseryJNI.createEntity()

    @OptIn(ExperimentalStdlibApi::class)
    operator fun <T: Any> get(type: String, cls: KClass<T>): T? { return cls.safeCast(MiseryJNI.getComponent(id, type)) }
    operator fun set(type: String, value: Any) { MiseryJNI.putComponent(id, type, value) }
}

fun test() {
    val entity0 = EntityJNI()
    val entity1 = EntityJNI()

    entity0["name"] = "alfred"
    entity1["name"] = "bobby"

    println(entity0["name", String::class])
}
