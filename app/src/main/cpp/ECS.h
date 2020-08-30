//
// Created by micha on 8/15/2020.
//

#ifndef MISERY_ECS_H
#define MISERY_ECS_H

#define MAX_COMPONENTS 64

#include <vector>
#include <unordered_map>
#include <string>
#include <jni.h>
#include <map>
#include "logging.h"

struct Entity {
    uint id;
    uint64_t signature = 0;
    uint* components = new uint[MAX_COMPONENTS];
    jobject jwrapper;
};

struct NativeComponentBase {
    virtual void* data() { return nullptr; }
    virtual ~NativeComponentBase() {}
};
template <typename T>
struct NativeComponentClass : NativeComponentBase {
    std::vector<T> components;
    void* data() { return &components; }
};

struct System {
    uint64_t signature;
    jobject jwrapper;
    jmethodID invoke;
};
struct NativeSystem {
    uint64_t signature;
    void (*apply)(Entity&, float);
};

class ECS {
    std::vector<Entity> entities;
    NativeComponentBase* components[MAX_COMPONENTS];
    std::vector<const char*> types;
    std::vector<System> systems;
    std::vector<NativeSystem> nativeSystems;

    uint64_t createSignature(int typeCount, const char* reqTypes[]);
public:
    uint newEntity(jobject jwrapper);
    Entity& getEntity(uint id);
    uint getTypeID(const char* type);
    template <typename T>
    void addComponent(uint entity, uint type, T& component);
    template <typename T>
    void addComponent(uint entity, const char* type, T& component);
    template <typename T>
    T* getComponent(uint entity, uint type);
    template<typename T>
    T* getComponent(uint entity, const char* type);
    template<typename T>
    bool removeComponent(uint entity, uint type);
    template<typename T>
    bool removeComponent(uint entity, const char* type);
    void addNativeSystem(void (*apply)(Entity&, float), int typeCount, const char* reqTypes[]);
    void addSystem(JNIEnv* env, jobject& jwrapper, jobjectArray& reqTypes);
    void update(JNIEnv* env, jfloat delta);
    void clear(JNIEnv* env);
};

namespace Misery { extern ECS ecs; }

template<typename T>
void ECS::addComponent(uint entity, uint type, T &component) {
    if (components[type] == nullptr)
        components[type] = new NativeComponentClass<T>();

    auto data = (std::vector<T>*) components[type]->data();
    entities[entity].components[type] = data->size();
    data->push_back(component);

    entities[entity].signature |= (0x1u << type);
}

template<typename T>
void ECS::addComponent(uint entity, const char* type, T &component)
{ addComponent(entity, getTypeID(type), component); }

template<typename T>
T *ECS::getComponent(uint entity, uint type) {
    uint component = entities[entity].components[type];
    auto data = (std::vector<T>*) components[type]->data();
    return data->data() + component;
}

template<typename T>
T *ECS::getComponent(uint entity, const char* type)
{ return getComponent<T>(entity, getTypeID(type)); }

template<typename T>
bool ECS::removeComponent(uint entity, const char *type) { return removeComponent<T>(entity, getTypeID(type)); }

template<typename T>
bool ECS::removeComponent(uint entity, uint type) {
//    auto data = (std::vector<T>*) components[type];
//    data->at(entities[entity].components[type]) = data->back();
//    data->pop_back();

    entities[entity].components[type] = 0;
    bool result = entities[entity].signature & 0x1u << type;
    entities[entity].signature ^= 0x1u << type;
    return result;
}

#endif //MISERY_ECS_H
