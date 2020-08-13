package com.klmn.misery

import java.lang.System.loadLibrary

object MiseryJNI {
    init { loadLibrary("misery-native-lib") }

    external fun helloWorld(): String
}