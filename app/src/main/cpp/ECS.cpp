//
// Created by micha on 8/15/2020.
//

#include "ECS.h"
#include <algorithm>
#include "logging.h"
#include "Transform.h"

namespace Misery { ECS ecs; }

uint ECS::newEntity(jobject jwrapper) {
    uint id = entities.size();
    entities.push_back(Entity { .id = id, .jwrapper = jwrapper });

    Transform transform;
    putComponent(id, "transform", transform);

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

uint64_t ECS::createSignature(int typeCount, const char* reqTypes[]) {
    uint64_t signature = 0;
    for (int i = 0; i < typeCount; i++)
        signature |= 0x1u << getTypeID(reqTypes[i]);
    return signature;
}

void ECS::addNativeSystem(void (*apply)(Entity&, float), int typeCount, const char* reqTypes[])
{ nativeSystems.push_back(NativeSystem {createSignature(typeCount, reqTypes), apply });}

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
    jmethodID jsysteminvoke = env->GetMethodID(jsystemclass, "invoke", "(Lcom/klmn/misery/update/Entity;F)V");
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
    for (auto& compClass : components) compClass->components.clear();
    for (auto& entity : entities) {
        delete[] entity.components;
        env->DeleteGlobalRef(entity.jwrapper);
    }
    for (auto& system : systems) env->DeleteGlobalRef(system.jwrapper);
    for (auto& type : types) delete[] type;
}

Entity &ECS::getEntity(uint id) { return entities[id]; }

void ECS::removeComponent(uint entity, const char *type) { return removeComponent(entity, getTypeID(type)); }
void ECS::removeComponent(uint entity, uint type) {
    if (!(entities[entity].signature & 0x1u << type)) return;

    std::vector<uint8_t>& data = components[type]->components;
    // the removed component
    size_t comp = entities[entity].components[type];
    // the last component in the vector, we move it to the position of the removed component
    size_t backComp = data.size() - components[type]->componentSize();

    components[type]->destroyComponent(comp);

    // if comp is not the last component, swap them
    if (comp != backComp) {
        std::memcpy(&data[comp], &data[backComp], components[type]->componentSize());
        // update any entities that refer to backComp
        for (auto& e : entities)
            if (e.components[type] == backComp) e.components[type] = comp;
    }
    data.resize(backComp);

    entities[entity].components[type] = 0;
    entities[entity].signature ^= 0x1u << type;
}
