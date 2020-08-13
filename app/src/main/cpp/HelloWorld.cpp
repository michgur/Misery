//
// Created by micha on 8/13/2020.
//

#include <jni.h>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

extern "C" JNIEXPORT jstring
Java_com_klmn_misery_MiseryJNI_helloWorld(JNIEnv *env, jobject ths) {
//    Assimp::Importer importer;
//    const aiScene* scene = importer.ReadFile( "./pig.obj",
//                                              aiProcess_CalcTangentSpace       |
//                                              aiProcess_Triangulate            |
//                                              aiProcess_JoinIdenticalVertices  |
//                                              aiProcess_SortByPType);
//
//    if( !scene) return env->NewStringUTF("failed");

    return env->NewStringUTF("success!");
}