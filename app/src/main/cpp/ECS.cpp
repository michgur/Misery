//
// Created by micha on 8/15/2020.
//

#include "ECS.h"
#include <algorithm>
#include "logging.h"
#include "Transform.h"

uint ECS::newEntity(jobject jwrapper) {
    uint id = entities.size();
    entities.push_back(Entity { .jwrapper = jwrapper });

    Transform transform;
    addNativeComponent(id, "transform", transform);

    return id;
}

uint ECS::getTypeID(const char* type) {
    for (uint i = 0; i < types.size(); i++)
        if (std::strcmp(type, types[i]) == 0) return i;

    uint len = std::strlen(type);
    char* allocType = new char[len + 1];
    std::strncpy(allocType, type, len); // copy, since strings fetched by JNI may be released
    allocType[len] = '\0';
    types.push_back(allocType);
    return types.size() - 1;
}

void ECS::addComponent(uint entity, const char* type, jobject& component) { addComponent(entity, getTypeID(type), component); }
void ECS::addComponent(uint entity, uint type, jobject& component) {
    entities[entity].components[type] = components[type].size();
    components[type].push_back(component);

    entities[entity].signature |= (0x1u << type);
}

jobject* ECS::getComponent(uint entity, const char* type) { return getComponent(entity, getTypeID(type)); }
jobject* ECS::getComponent(uint entity, uint type) {
    uint component = entities[entity].components[type];
    return &components[type][component];
}

uint64_t ECS::createSignature(uint64_t signature, const char* arg) { return signature | 0x1u << getTypeID(arg); }

template<typename ...T>
uint64_t ECS::createSignature(uint64_t signature, const char* arg, T&... args) {
    signature = createSignature(signature, arg);
    return createSignature(signature, args...);
}

void ECS::addNativeSystem(void (*apply)(Entity&, float), const char* reqType0, const char* reqTypes, ...)
{ nativeSystems.push_back(NativeSystem {createSignature(0, reqType0, reqTypes), apply });}

ECS &ECS::getInstance() {
    static ECS instance;
    return instance;
}

void ECS::addSystem(JNIEnv* env, jobject &jwrapper, jobjectArray &reqTypes) {
    uint64_t signature = 0;
    uint typeCount = env->GetArrayLength(reqTypes);
    for (uint i = 0; i < typeCount; i++) {
        auto typeString = (jstring) env->GetObjectArrayElement(reqTypes, i);
        const char* typeChars = env->GetStringUTFChars(typeString, nullptr);

        signature |= 0x1u << getTypeID(typeChars);

        env->ReleaseStringUTFChars(typeString, typeChars);
    }
    jclass jsystemclass = env->GetObjectClass(jwrapper);
    jmethodID jsysteminvoke = env->GetMethodID(jsystemclass, "invoke", "(Lcom/klmn/misery/Entity;F)V");
    systems.push_back(System { signature, env->NewGlobalRef(jwrapper), jsysteminvoke });
}

#define MATCHES(s, e) (s.signature & e.signature) == s.signature
void ECS::update(JNIEnv* env, jfloat delta) {
    for (auto & system : nativeSystems) {
        for (auto & entity : entities)
            if (MATCHES(system, entity)) system.apply(entity, delta);
    }
    for (auto& system : systems) {
        for (auto& entity : entities)
            if (MATCHES(system, entity))
                env->CallVoidMethod(system.jwrapper, system.invoke, entity.jwrapper, delta);
    }
}

void ECS::clear(JNIEnv* env) {
//    for (auto& pair : nativeComponents) delete pair.second;
    for (auto& vec : components)
        for (auto& comp : vec) env->DeleteGlobalRef(comp);
    for (auto& entity : entities) {
        delete[] entity.components;
        env->DeleteGlobalRef(entity.jwrapper);
    }
    for (auto& system : systems) env->DeleteGlobalRef(system.jwrapper);
    for (auto& type : types) delete[] type;
}
