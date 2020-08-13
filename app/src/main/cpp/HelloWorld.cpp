//
// Created by micha on 8/13/2020.
//

#include <jni.h>
#include <string>

extern "C" JNIEXPORT jstring JNICALL
Java_com_klmn_misery_MiseryJNI_helloWorld(JNIEnv *env, jobject thiz) {
    return env->NewStringUTF("Hello from JNI!");
}