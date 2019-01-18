package com.klmn.misery

import android.view.MotionEvent

/**
 * ಠ^ಠ.
 * Created by Michael on 1/18/2019.
 */
class TouchControls(vararg pairs: Pair<Int, Entity.(MotionEvent) -> Unit>)
{
    private val controls = mutableMapOf(*pairs)

    operator fun get(action: Int) = controls[action]
    operator fun set(action: Int, control: Entity.(MotionEvent) -> Unit) { controls[action] = control }

    fun onTouchEvent(entity: Entity, event: MotionEvent) = controls[event.action]?.invoke(entity, event)
}
