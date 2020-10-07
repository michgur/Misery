//
// Created by micha on 8/15/2020.
//

#include "ECS.h"
#include <algorithm>
#include "logging.h"
#include "Transform.h"

namespace Misery { ECS ecs; }

uint ECS::newEntity(jobject jwrapper) {
    uint id;
    if (removedEntities.empty()) {
        id = entities.size();
        entities.resize(entities.size() + 1);
    }
    else {
        id = removedEntities.back();
        removedEntities.pop_back();
    }

    entities[id] = Entity { .id = id, .jwrapper = jwrapper };

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
uint64_t ECS::createSignature(JNIEnv* env, jobjectArray &reqTypes) {
    uint64_t signature = 0;
    uint typeCount = env->GetArrayLength(reqTypes);
    for (uint i = 0; i < typeCount; i++) {
        auto typeString = (jstring) env->GetObjectArrayElement(reqTypes, i);
        const char* typeChars = env->GetStringUTFChars(typeString, nullptr);

        signature |= 0x1u << getTypeID(typeChars);

        env->ReleaseStringUTFChars(typeString, typeChars);
    }
    return signature;
}

void ECS::addSystem(void (*apply)(uint, float), int typeCount, const char **reqTypes)
{ nativeSystems.push_back(NativeSystem {createSignature(typeCount, reqTypes), apply });}

void ECS::addSystem(JNIEnv* env, jobject &jwrapper, jobjectArray &reqTypes) {
    jclass jsystemclass = env->GetObjectClass(jwrapper);
    jmethodID jsysteminvoke = env->GetMethodID(jsystemclass, "invoke", "(Lcom/klmn/misery/update/Entity;F)V");
    systems.push_back(System { createSignature(env, reqTypes), env->NewGlobalRef(jwrapper), jsysteminvoke });
}

#define MATCHES(s, e) (s.signature & e.signature) == s.signature
void ECS::update(JNIEnv* env, jfloat delta) {
    for (auto & system : nativeSystems) {
        for (auto & entity : entities)
            if (MATCHES(system, entity)) system.apply(entity.id, delta);
    }
    for (auto& system : systems) {
        for (auto& entity : entities)
            if (MATCHES(system, entity)) {
                env->CallVoidMethod(system.jwrapper, system.invoke, entity.jwrapper, delta);
                JNI_CATCH_EXCEPTION("java exception occurred when trying to run system %p on entity %i",
                            &system, entity.id);
            }
    }
}

void ECS::clear(JNIEnv* env) {
    for (auto& compClass : components) {
        compClass->components.clear();
        delete compClass;
    }
    for (auto& entity : entities) {
        entity.components.clear();
        if (entity.jwrapper != nullptr) env->DeleteGlobalRef(entity.jwrapper);
    }
    for (auto& system : systems) env->DeleteGlobalRef(system.jwrapper);
    for (auto& type : types) delete[] type;
}

Entity &ECS::getEntity(uint entity) { return entities[entity]; }

void ECS::removeComponent(uint entity, const char *type) { return removeComponent(entity, getTypeID(type)); }
void ECS::removeComponent(uint entity, uint type) {
    if (!(entities[entity].signature & 0x1u << type)) return;

    // invoke corresponding listeners
    for (auto listener : listeners)
        if (listener->signature & 0x1u << type) listener->onRemoveComponent(entity, type);

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

void ECS::removeEntity(JNIEnv *env, uint entity) {
    // delete entity data
    for (auto it : entities[entity].components) {
    // todo: figure out when to delete the components, maybe reference-based system
//        auto compClass = dynamic_cast<ComponentClass<jobject>*>(components[it.first]);
//        if (compClass != nullptr)
//            env->DeleteGlobalRef(*(jobject*) &compClass->components[it.second]);
        removeComponent(entity, it.first);
    }

    env->DeleteGlobalRef(entities[entity].jwrapper);
    if (entity == entities.size() - 1) entities.pop_back();
    else {
        //disable the entity, override it when possible
        entities[entity].signature = 0;
        entities[entity].jwrapper = nullptr;
        removedEntities.push_back(entity);
    }
}

void ECS::removeSystem(void (*apply)(uint, float)) {
    for (uint i = 0; i < nativeSystems.size(); i++)
        if (nativeSystems[i].apply == apply) {
            nativeSystems.erase(nativeSystems.begin() + i);
            break;
        }
}

// tbh I don't think this works, should probably index systems by integer ids
void ECS::removeSystem(JNIEnv *env, jobject &jwrapper) {
    for (uint i = 0; i < systems.size(); i++) {
        if (systems[i].jwrapper == jwrapper) {
            env->DeleteGlobalRef(jwrapper);
            systems.erase(systems.begin() + i);
            break;
        }
    }
}

bool ECS::hasComponent(uint entity, const char *type) { return hasComponent(entity, getTypeID(type)); }
bool ECS::hasComponent(uint entity, uint type) {
    return entities[entity].signature & 0x1u << type;
}

bool ECS::entityExists(uint entity) { return entity < entities.size() && entities[entity].signature != 0; }
