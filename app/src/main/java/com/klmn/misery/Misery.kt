package com.klmn.misery

import android.app.Activity
import android.app.ActivityManager
import android.content.Context
import android.os.Bundle

/**
 * ಠ^ಠ.
 * Created by Michael on 12/14/2018.
 */
class Misery : Activity()
{
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        val info = (getSystemService(Context.ACTIVITY_SERVICE) as ActivityManager).deviceConfigurationInfo
        assert(info.reqGlEsVersion < 0x30000) { "Device Does Not Support GLES3.0" }

        setContentView(MiseryView(this))
    }
}
