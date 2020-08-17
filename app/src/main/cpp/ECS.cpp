//
// Created by micha on 8/15/2020.
//

#include "ECS.h"
#include <algorithm>
#include "logging.h"

uint ECS::newEntity() {
    Entity entity;
    entity.id = entities.size();
    entities.push_back(entity);
    return entity.id;
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

void ECS::addSystem(JNIEnv* env, jobject &apply, jobjectArray &reqTypes) {
    uint64_t signature = 0;
    uint typeCount = env->GetArrayLength(reqTypes);
    for (uint i = 0; i < typeCount; i++) {
        auto typeString = (jstring) env->GetObjectArrayElement(reqTypes, i);
        const char* typeChars = env->GetStringUTFChars(typeString, nullptr);

        std::string st = std::string(typeChars);
        signature |= 0x1u << getTypeID(st);

        env->ReleaseStringUTFChars(typeString, typeChars);
    }
    systems.push_back(System { signature, env->NewGlobalRef(apply) });
}

void ECS::update(JNIEnv* env, jfloat delta) {
    for (uint i = 0; i < nativeSystems.size(); i++) {
        NativeSystem& system = nativeSystems[i];
        for (uint j = 0; j < entities.size(); j++)
            if ((entities[i].signature & system.signature) == system.signature) system.apply(entities[i], delta);
    }
    if (systems.empty()) return;
    jclass jentityclass = env->FindClass("com/klmn/misery/Entity");
    jmethodID jentityinit = env->GetMethodID(jentityclass, "<init>", "(I)V");
    for (auto& system : systems) {
        for (auto& entity : entities)
            if ((entity.signature & system.signature) == system.signature) {
                jclass jsystemclass = env->GetObjectClass(system.apply);
                jmethodID jsysteminvoke = env->GetMethodID(jsystemclass, "invoke", "(Lcom/klmn/misery/Entity;F)V");
                jobject entityObject = env->NewObject(jentityclass, jentityinit, (jint) entity.id);
                env->CallVoidMethod(system.apply, jsysteminvoke, entityObject, delta);
                env->DeleteLocalRef(jsystemclass);
                env->DeleteLocalRef(entityObject);
            }
    }
}
