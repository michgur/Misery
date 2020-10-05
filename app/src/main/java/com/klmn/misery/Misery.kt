package com.klmn.misery

import android.app.Activity
import android.os.Bundle
import com.klmn.misery.examples.FlappyPig
import com.klmn.misery.jni.MiseryJNI
import com.klmn.misery.render.MiserySurfaceView

/**
 * ಠ^ಠ.
 * Created by Michael on 12/14/2018.
 */

class Misery : Activity()
{
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        val view = MiserySurfaceView(this)
        setContentView(view)
        FlappyPig(this).init()
    }

    override fun onResume() {
        super.onResume()
        MiseryJNI.startRenderThread()
    }

    override fun onPause() {
        super.onPause()
        MiseryJNI.killRenderThread()
    }

    override fun onDestroy() {
        MiseryJNI.clearECS()
        super.onDestroy()
    }
}
