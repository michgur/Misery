package com.klmn.misery

import android.app.Activity
import android.os.Bundle

/**
 * ಠ^ಠ.
 * Created by Michael on 12/14/2018.
 */
class Misery : Activity()
{
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        val view = MiseryView(PigGame(this))
        setContentView(view)
    }
}
