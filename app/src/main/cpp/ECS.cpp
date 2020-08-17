//
// Created by micha on 8/15/2020.
//

#include "ECS.h"
#include <algorithm>
#include "logging.h"

uint ECS::newEntity(jobject jwrapper) {
    uint id = entities.size();
    entities.push_back(Entity { id, 0, std::map<uint, uint>(), jwrapper });
    return id;
}

uint ECS::getTypeID(std::string &type) {
    auto i = std::find(types.begin(), types.end(), type);
    if (i == types.end()) {
        types.push_back(type);
        return types.size() - 1;
    } else return i - types.begin();
}

void ECS::addComponent(uint entity, std::string& type, jobject& component) { addComponent(entity, getTypeID(type), component); }
void ECS::addComponent(uint entity, uint type, jobject& component) {
    entities[entity].components[type] = components[type].size();
    components[type].push_back(component);

    entities[entity].signature |= (0x1u << type);
}

jobject* ECS::getComponent(uint entity, std::string& type) { return getComponent(entity, getTypeID(type)); }
jobject* ECS::getComponent(uint entity, uint type) {
    uint component = entities[entity].components[type];
    return &components[type][component];
}

uint64_t ECS::createSignature(uint64_t signature, std::string& arg) { return signature | 0x1u << getTypeID(arg); }

template<typename ...T>
uint64_t ECS::createSignature(uint64_t signature, std::string& arg, T&... args) {
    signature = createSignature(signature, arg);
    return createSignature(signature, args...);
}

void ECS::addNativeSystem(void (*apply)(Entity&, float), std::string& reqType0, std::string& reqTypes, ...)
{ nativeSystems.push_back(NativeSystem {createSignature(0, reqType0, reqTypes), apply });}

ECS &ECS::getInstance() {
    static ECS instance;
    return instance;
}

template<typename T>
void ECS::addNativeComponent(uint entity, uint type, T &component) {
    auto it = nativeComponents.find(type);
    if (it == nativeComponents.end()) nativeComponents[type] = NativeComponentClass<T>();
    std::vector<T> data = nativeComponents[type].data();
    entities[entity].components[type] = data;
    data.push_back(component);

    entities[entity].signature |= (0x1u << type);
}

template<typename T>
void ECS::addNativeComponent(uint entity, std::string& type, T &component)
{ addNativeComponent(entity, getTypeID(type), component); }

template<typename T>
T *ECS::getNativeComponent(uint entity, uint type)
{ return ((std::vector<T>) nativeComponents[type].data())[entity]; }

template<typename T>
T *ECS::getNativeComponent(uint entity, std::string& type)
{ return getNativeComponent<T>(entity, getTypeID(type)); }

void ECS::addSystem(JNIEnv* env, jobject &jwrapper, jobjectArray &reqTypes) {
    uint64_t signature = 0;
    uint typeCount = env->GetArrayLength(reqTypes);
    for (uint i = 0; i < typeCount; i++) {
        auto typeString = (jstring) env->GetObjectArrayElement(reqTypes, i);
        const char* typeChars = env->GetStringUTFChars(typeString, nullptr);

        std::string st = std::string(typeChars);
        signature |= 0x1u << getTypeID(st);

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
    for (auto & vec : components)
        for (auto& comp : vec) env->DeleteGlobalRef(comp);
    for (auto& entity : entities) env->DeleteGlobalRef(entity.jwrapper);
    for (auto& system : systems) env->DeleteGlobalRef(system.jwrapper);
}
