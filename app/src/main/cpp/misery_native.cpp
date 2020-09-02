//
// Created by micha on 8/13/2020.
//

#include <jni.h>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/port/AndroidJNI/AndroidJNIIOSystem.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>
#include <GLES3/gl3.h>
#include "logging.h"
#include "ECS.h"
#include "Transform.h"
#include "assets.h"
#include "RenderContext.h"
#include "interaction.h"
#include "physics.h"


extern "C"
JNIEXPORT void JNICALL
Java_com_klmn_misery_jni_MiseryJNI_setNativeAssetManager(JNIEnv *env, jobject thiz,
                                                                 jobject asset_manager) {
    Misery::assetManager = AAssetManager_fromJava(env, asset_manager);

    const char* types[] = { "transform", "mesh", "material" };
    Misery::ecs.addSystem(Misery::renderContext.render, 3, types);
    const char* motionTypes[] = { "transform", "motion" };
    Misery::ecs.addSystem(motionSystem, 2, motionTypes);
}


extern "C"
JNIEXPORT jlong JNICALL
Java_com_klmn_misery_jni_MiseryJNI_loadMesh(JNIEnv *env, jobject thiz,
                                                  jstring file, jstring ext) {
    const char* fileString = env->GetStringUTFChars(file, nullptr);
    const char* extString = env->GetStringUTFChars(ext, nullptr);
    Misery::Mesh* mesh = Misery::loadMeshFromAsset(fileString, extString);
    env->ReleaseStringUTFChars(file, fileString);
    env->ReleaseStringUTFChars(ext, extString);
    return (long) mesh;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_klmn_misery_jni_MiseryJNI_drawMesh(JNIEnv *env, jobject thiz, jlong pointer) {
    glBindVertexArray(((Misery::Mesh*) pointer)->vao);
    glDrawElements(GL_TRIANGLES, ((Misery::Mesh*) pointer)->size, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_klmn_misery_jni_MiseryJNI_createProgram(JNIEnv *env, jobject thiz,
                                                         jstring vertex_file,
                                                         jstring fragment_file) {
    const char* vertexFileName = env->GetStringUTFChars(vertex_file, nullptr);
    const char* fragmentFileName = env->GetStringUTFChars(fragment_file, nullptr);
    uint id = Misery::createShaderProgram(vertexFileName, fragmentFileName);
    env->ReleaseStringUTFChars(vertex_file, vertexFileName);
    env->ReleaseStringUTFChars(fragment_file, fragmentFileName);
    return id;
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_klmn_misery_jni_MiseryJNI_createEntity(JNIEnv *env, jobject thiz, jobject wrapper) {
    return Misery::ecs.newEntity(env->NewGlobalRef(wrapper));
}
extern "C"
JNIEXPORT void JNICALL
Java_com_klmn_misery_jni_MiseryJNI_putComponent(JNIEnv *env, jobject thiz, jint entity, jstring type,
                                            jobject value) {
    const char* typeChars = env->GetStringUTFChars(type, nullptr);
    jobject allocValue = env->NewGlobalRef(value);
    Misery::ecs.putComponent(entity, typeChars, allocValue);
    env->ReleaseStringUTFChars(type, typeChars);
}
extern "C"
JNIEXPORT jobject JNICALL
Java_com_klmn_misery_jni_MiseryJNI_getComponent(JNIEnv *env, jobject thiz, jint entity, jstring type) {
    const char* typeChars = env->GetStringUTFChars(type, nullptr);
    auto cmp = Misery::ecs.getComponent<jobject>(entity, typeChars);
    env->ReleaseStringUTFChars(type, typeChars);
    return *cmp;
}extern "C"
JNIEXPORT void JNICALL
Java_com_klmn_misery_jni_MiseryJNI_addSystem(JNIEnv *env, jobject thiz, jobjectArray types,
                                         jobject apply) {
    Misery::ecs.addSystem(env, apply, types);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_klmn_misery_jni_MiseryJNI_updateECS(JNIEnv *env, jobject thiz, jfloat delta) {
    Misery::ecs.update(env, delta);
    Misery::interactions.update(env, delta);
}extern "C"
JNIEXPORT void JNICALL
Java_com_klmn_misery_jni_MiseryJNI_clearECS(JNIEnv *env, jobject thiz) {
    Misery::ecs.clear(env);
    Misery::interactions.clear(env);
}

extern "C"
JNIEXPORT jfloat JNICALL
Java_com_klmn_misery_jni_MiseryJNI_getFloat(JNIEnv *env, jobject thiz, jlong pointer, jint offset) {
    return ((float*) pointer)[offset];
}extern "C"
JNIEXPORT void JNICALL
Java_com_klmn_misery_jni_MiseryJNI_setFloat(JNIEnv *env, jobject thiz, jlong pointer, jfloat f,
                                        jint offset) {
    ((float*) pointer)[offset] = f;
}extern "C"
JNIEXPORT jfloatArray JNICALL
Java_com_klmn_misery_jni_MiseryJNI_getFloats(JNIEnv *env, jobject thiz, jlong pointer, jint count,
                                         jint offset) {
    jfloatArray array = env->NewFloatArray(count);
    env->SetFloatArrayRegion(array, 0, count, (float*) pointer + offset);
    return array;
}extern "C"
JNIEXPORT void JNICALL
Java_com_klmn_misery_jni_MiseryJNI_setFloats(JNIEnv *env, jobject thiz, jlong pointer, jfloatArray f,
                                         jint offset) {
    env->GetFloatArrayRegion(f, 0, env->GetArrayLength(f), (float*) pointer + offset);
}extern "C"
JNIEXPORT jlong JNICALL
Java_com_klmn_misery_jni_MiseryJNI_getTransformComponent(JNIEnv *env, jobject thiz, jint entity) {
    auto* t = Misery::ecs.getComponent<Transform>(entity, "transform");
    return (long) t->data;
}extern "C"
JNIEXPORT void JNICALL
Java_com_klmn_misery_jni_MiseryJNI_putTransformComponent(JNIEnv *env, jobject thiz, jint entity,
                                                         jfloatArray f) {
    auto* t = Misery::ecs.getComponent<Transform>(entity, "transform");
    env->GetFloatArrayRegion(f, 0, TRANSFORM_SIZE, t->data);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_klmn_misery_jni_MiseryJNI_putMaterialComponent(JNIEnv *env, jobject thiz, jint entity,
                                                        jint material) {
    Misery::ecs.removeComponent(material, "transform");
    uint ref = (uint) material;
    Misery::ecs.putComponent<uint>(entity, "material", ref);
}extern "C"
JNIEXPORT jobject JNICALL
Java_com_klmn_misery_jni_MiseryJNI_getMaterialComponent(JNIEnv *env, jobject thiz, jint entity) {
    uint id = *Misery::ecs.getComponent<uint>(entity, "material");
    return Misery::ecs.getEntity(id).jwrapper;
}
extern "C"
JNIEXPORT void JNICALL
Java_com_klmn_misery_jni_MiseryJNI_putIntComponent(JNIEnv *env, jobject thiz, jint entity, jstring type, jint value) {
    const char* typeChars = env->GetStringUTFChars(type, nullptr);
    Misery::ecs.putComponent<int>(entity, typeChars, value);
    env->ReleaseStringUTFChars(type, typeChars);
}extern "C"
JNIEXPORT void JNICALL
Java_com_klmn_misery_jni_MiseryJNI_putLongComponent(JNIEnv *env, jobject thiz, jint entity,
                                                jstring type, jlong value) {
    const char* typeChars = env->GetStringUTFChars(type, nullptr);
    long ref = value;
    Misery::ecs.putComponent<long>(entity, typeChars, ref);
    env->ReleaseStringUTFChars(type, typeChars);
}extern "C"
JNIEXPORT void JNICALL
Java_com_klmn_misery_jni_MiseryJNI_setViewSize(JNIEnv *env, jobject thiz, jint width, jint height) {
    Misery::renderContext.setViewSize((uint) width, (uint) height);
}extern "C"
JNIEXPORT jlong JNICALL
Java_com_klmn_misery_jni_MiseryJNI_getCameraTransform(JNIEnv *env, jobject thiz) {
    return (long) &Misery::renderContext.camera;
}extern "C"
JNIEXPORT void JNICALL
Java_com_klmn_misery_jni_MiseryJNI_removeComponent(JNIEnv *env, jobject thiz, jint entity,
                                               jstring type) {
    const char* typeChars = env->GetStringUTFChars(type, nullptr);
    env->DeleteGlobalRef(*Misery::ecs.getComponent<jobject>(entity, typeChars));
    Misery::ecs.removeComponent(entity, typeChars);
    env->ReleaseStringUTFChars(type, typeChars);
}extern "C"
JNIEXPORT void JNICALL
Java_com_klmn_misery_jni_MiseryJNI_removeEntity(JNIEnv *env, jobject thiz, jint entity) {
    Misery::ecs.removeEntity(env, entity);
}extern "C"
JNIEXPORT void JNICALL
Java_com_klmn_misery_jni_MiseryJNI_putAABBComponent(JNIEnv *env, jobject thiz, jint entity,
                                                    jfloatArray aabb) {
    AABB component;
    env->GetFloatArrayRegion(aabb, 0, 6, component.data);
    Misery::ecs.putComponent(entity, "aabb", component);
}extern "C"
JNIEXPORT void JNICALL
Java_com_klmn_misery_jni_MiseryJNI_addInteraction(JNIEnv *env, jobject thiz, jobjectArray active_types,
                                              jobjectArray passive_types, jobject apply) {
    Misery::interactions.addInteraction(env, apply, active_types, passive_types);
}extern "C"
JNIEXPORT jlong JNICALL
Java_com_klmn_misery_jni_MiseryJNI_getComponentPointer(JNIEnv *env, jobject thiz, jint entity,
                                                   jstring type) {
    const char* typeChars = env->GetStringUTFChars(type, nullptr);
    void* result = Misery::ecs.getComponent<void>(entity, typeChars);
    env->ReleaseStringUTFChars(type, typeChars);
    return (long) result;
}
extern "C"
JNIEXPORT void JNICALL
Java_com_klmn_misery_jni_MiseryJNI_putMotionComponent(JNIEnv *env, jobject thiz, jint entity,
                                                      jfloatArray motion) {
    Motion component;
    env->GetFloatArrayRegion(motion, 0, 6, component.data);
    Misery::ecs.putComponent(entity, "motion", component);
}