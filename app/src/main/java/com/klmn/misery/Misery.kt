package com.klmn.misery

import android.app.Activity
import android.os.Bundle
import com.klmn.misery.examples.FlappyPig
import com.klmn.misery.jni.MiseryJNI
import com.klmn.misery.render.MiserySurfaceView
import com.klmn.misery.render.MiseryView

/**
 * ಠ^ಠ.
 * Created by Michael on 12/14/2018.
 */
const val NATIVE_SURFACE = true

class Misery : Activity()
{
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        if (!NATIVE_SURFACE) {
            val view = MiseryView(FlappyPig(this))
            setContentView(view)
        } else {
            val view = MiserySurfaceView(this)
            setContentView(view)
            FlappyPig(this).init()
        }
    }

    override fun onDestroy() {
        MiseryJNI.clearECS()
        super.onDestroy()
    }
}
