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

void render(Entity &e, float delta) {
    uint material = *Misery::ecs.getComponent<uint>(e.id, "material");
    int shader = *Misery::ecs.getComponent<int>(material, "shader");
    glUseProgram(shader);

    int mvp = glGetUniformLocation(shader, "mvp");
    int diffuse = glGetUniformLocation(shader, "diffuse");

    matrix4f m = Misery::ecs.getComponent<Transform>(e.id, "transform")->toMatrix();
    m = Misery::renderContext.projection * (Misery::renderContext.camera.toMatrix() * m);
    glUniformMatrix4fv(mvp, 1, true, m[0]);
    glActiveTexture(GL_TEXTURE0);
    uint texture = *Misery::ecs.getComponent<int>(material, "diffuse");
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(diffuse, 0);

    auto* mesh = (Misery::Mesh*) *Misery::ecs.getComponent<long>(e.id, "mesh");
    glBindVertexArray(mesh->vao);
    glDrawElements(GL_TRIANGLES, mesh->size, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_klmn_misery_MiseryJNI_setNativeAssetManager(JNIEnv *env, jobject thiz,
                                                                 jobject asset_manager) {
    Misery::assetManager = AAssetManager_fromJava(env, asset_manager);

    const char* types[] = { "transform", "mesh", "material" };
    Misery::ecs.addNativeSystem(render, 3, types);
}


extern "C"
JNIEXPORT jlong JNICALL
Java_com_klmn_misery_MiseryJNI_loadMesh(JNIEnv *env, jobject thiz,
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
Java_com_klmn_misery_MiseryJNI_drawMesh(JNIEnv *env, jobject thiz, jlong pointer) {
    glBindVertexArray(((Misery::Mesh*) pointer)->vao);
    glDrawElements(GL_TRIANGLES, ((Misery::Mesh*) pointer)->size, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_klmn_misery_MiseryJNI_createProgram(JNIEnv *env, jobject thiz,
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
Java_com_klmn_misery_MiseryJNI_createEntity(JNIEnv *env, jobject thiz, jobject wrapper) {
    return Misery::ecs.newEntity(env->NewGlobalRef(wrapper));
}
extern "C"
JNIEXPORT void JNICALL
Java_com_klmn_misery_MiseryJNI_putComponent(JNIEnv *env, jobject thiz, jint entity, jstring type,
                                            jobject value) {
    const char* typeChars = env->GetStringUTFChars(type, nullptr);
    jobject allocValue = env->NewGlobalRef(value);
    Misery::ecs.putComponent(entity, typeChars, allocValue);
    env->ReleaseStringUTFChars(type, typeChars);
}
extern "C"
JNIEXPORT jobject JNICALL
Java_com_klmn_misery_MiseryJNI_getComponent(JNIEnv *env, jobject thiz, jint entity, jstring type) {
    const char* typeChars = env->GetStringUTFChars(type, nullptr);
    auto cmp = Misery::ecs.getComponent<jobject>(entity, typeChars);
    env->ReleaseStringUTFChars(type, typeChars);
    return *cmp;
}extern "C"
JNIEXPORT void JNICALL
Java_com_klmn_misery_MiseryJNI_addSystem(JNIEnv *env, jobject thiz, jobjectArray types,
                                         jobject apply) {
    Misery::ecs.addSystem(env, apply, types);
}extern "C"
JNIEXPORT void JNICALL
Java_com_klmn_misery_MiseryJNI_updateECS(JNIEnv *env, jobject thiz, jfloat delta) {
    Misery::ecs.update(env, delta);
}extern "C"
JNIEXPORT void JNICALL
Java_com_klmn_misery_MiseryJNI_clearECS(JNIEnv *env, jobject thiz) {
    Misery::ecs.clear(env);
}

extern "C"
JNIEXPORT jfloat JNICALL
Java_com_klmn_misery_MiseryJNI_getFloat(JNIEnv *env, jobject thiz, jlong pointer, jint offset) {
    return ((float*) pointer)[offset];
}extern "C"
JNIEXPORT void JNICALL
Java_com_klmn_misery_MiseryJNI_setFloat(JNIEnv *env, jobject thiz, jlong pointer, jfloat f,
                                        jint offset) {
    ((float*) pointer)[offset] = f;
}extern "C"
JNIEXPORT jfloatArray JNICALL
Java_com_klmn_misery_MiseryJNI_getFloats(JNIEnv *env, jobject thiz, jlong pointer, jint count,
                                         jint offset) {
    jfloatArray array = env->NewFloatArray(count);
    env->SetFloatArrayRegion(array, 0, count, (float*) pointer + offset);
    return array;
}extern "C"
JNIEXPORT void JNICALL
Java_com_klmn_misery_MiseryJNI_setFloats(JNIEnv *env, jobject thiz, jlong pointer, jfloatArray f,
                                         jint offset) {
    env->GetFloatArrayRegion(f, 0, env->GetArrayLength(f), (float*) pointer + offset);
}extern "C"
JNIEXPORT jlong JNICALL
Java_com_klmn_misery_MiseryJNI_getTransformComponent(JNIEnv *env, jobject thiz, jint entity) {
    auto* t = Misery::ecs.getComponent<Transform>(entity, "transform");
    return (long) t->data;
}extern "C"
JNIEXPORT void JNICALL
Java_com_klmn_misery_MiseryJNI_setTransformComponent__I_3F(JNIEnv *env, jobject thiz, jint entity,
                                                           jfloatArray f) {
    auto* t = Misery::ecs.getComponent<Transform>(entity, "transform");
    env->GetFloatArrayRegion(f, 0, TRANSFORM_SIZE, t->data);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_klmn_misery_MiseryJNI_setMaterialComponent(JNIEnv *env, jobject thiz, jint entity,
                                                    jint material) {
    Misery::ecs.removeComponent(material, "transform");
    uint ref = (uint) material;
    Misery::ecs.putComponent<uint>(entity, "material", ref);
}extern "C"
JNIEXPORT jobject JNICALL
Java_com_klmn_misery_MiseryJNI_getMaterialComponent(JNIEnv *env, jobject thiz, jint entity) {
    uint id = *Misery::ecs.getComponent<uint>(entity, "material");
    return Misery::ecs.getEntity(id).jwrapper;
}
extern "C"
JNIEXPORT void JNICALL
Java_com_klmn_misery_MiseryJNI_setIntComponent(JNIEnv *env, jobject thiz, jint entity, jstring type, jint value) {
    const char* typeChars = env->GetStringUTFChars(type, nullptr);
    Misery::ecs.putComponent<int>(entity, typeChars, value);
    env->ReleaseStringUTFChars(type, typeChars);
}extern "C"
JNIEXPORT void JNICALL
Java_com_klmn_misery_MiseryJNI_setLongComponent(JNIEnv *env, jobject thiz, jint entity,
                                                jstring type, jlong value) {
    const char* typeChars = env->GetStringUTFChars(type, nullptr);
    Misery::ecs.putComponent<long>(entity, typeChars, value);
    env->ReleaseStringUTFChars(type, typeChars);
}extern "C"
JNIEXPORT void JNICALL
Java_com_klmn_misery_MiseryJNI_setViewSize(JNIEnv *env, jobject thiz, jint width, jint height) {
    Misery::renderContext.setViewSize((uint) width, (uint) height);
}extern "C"
JNIEXPORT jlong JNICALL
Java_com_klmn_misery_MiseryJNI_getCameraTransform(JNIEnv *env, jobject thiz) {
    return (long) &Misery::renderContext.camera;
}extern "C"
JNIEXPORT void JNICALL
Java_com_klmn_misery_MiseryJNI_removeComponent(JNIEnv *env, jobject thiz, jint entity,
                                               jstring type) {
    const char* typeChars = env->GetStringUTFChars(type, nullptr);
    env->DeleteGlobalRef(*Misery::ecs.getComponent<jobject>(entity, typeChars));
    Misery::ecs.removeComponent(entity, typeChars);
    env->ReleaseStringUTFChars(type, typeChars);
}extern "C"
JNIEXPORT void JNICALL
Java_com_klmn_misery_MiseryJNI_removeEntity(JNIEnv *env, jobject thiz, jint entity) {
    Misery::ecs.removeEntity(env, entity);
}