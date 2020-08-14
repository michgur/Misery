package com.klmn.misery

import android.app.Activity
import android.content.res.AssetManager
import android.os.Bundle
import java.lang.System

/**
 * ಠ^ಠ.
 * Created by Michael on 12/14/2018.
 */
class Misery : Activity()
{
    companion object {
        init { System.loadLibrary("misery-native-lib") }
        external fun setNativeAssetManager(assetManager: AssetManager)
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        val view = MiseryView(PigGame(this))
        setContentView(view)

        setNativeAssetManager(assets)
    }
}
