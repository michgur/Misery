package com.klmn.misery

import android.app.Activity
import android.os.Bundle
import android.os.Handler
import android.os.Looper
import com.klmn.misery.examples.FlappyPig
import com.klmn.misery.jni.MiseryJNI
import com.klmn.misery.render.MiserySurfaceView
import kotlin.concurrent.thread

/**
 * ಠ^ಠ.
 * Created by Michael on 12/14/2018.
 */

class Misery : Activity()
{
    private var running = true
    private val update = Runnable {
        var last = System.nanoTime()
        while (running) {
            val now = System.nanoTime()
            MiseryJNI.updateECS((now - last).toFloat() / 1000000000)
            last = now
        }
    }
    private lateinit var thread: Thread

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        val view = MiserySurfaceView(this)
        setContentView(view)
        FlappyPig(this).init()
    }

    override fun onResume() {
        super.onResume()
        MiseryJNI.startRenderThread()
        thread = Thread(update)
        thread.start()
    }

    override fun onPause() {
        super.onPause()
        MiseryJNI.killRenderThread()
        running = false
        thread.join()
    }

    override fun onDestroy() {
        MiseryJNI.clearECS()
        super.onDestroy()
    }
}
