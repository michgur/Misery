package com.klmn.misery

import android.app.Activity
import android.os.Bundle
import android.view.MotionEvent
import com.klmn.misery.examples.FlappyPig
import com.klmn.misery.jni.MiseryJNI
import com.klmn.misery.render.MiserySurfaceView
import kotlin.concurrent.withLock

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
            MiseryJNI.updateECS((now - last).toFloat() / 1_000_000_000)
            last = now
        }
    }
    private lateinit var thread: Thread

    private val game = FlappyPig(this)
    override fun onTouchEvent(event: MotionEvent?): Boolean {
        return if (event != null) {
            game.inputLock.withLock { game.inputBuffer.add(event) }
            true
        }
        else false
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        val view = MiserySurfaceView(this)
        setContentView(view)
        game.init()
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
