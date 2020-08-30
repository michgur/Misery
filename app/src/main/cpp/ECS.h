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
    size_t* components = new size_t[MAX_COMPONENTS];
    jobject jwrapper;
};

struct ComponentClassBase {
    std::vector<uint8_t> components;

    virtual size_t componentSize() { return 1; }
    virtual void destroyComponent(size_t index) {}
};
template <typename T>
struct ComponentClass : ComponentClassBase {
    size_t componentSize() { return sizeof(T); }
    void destroyComponent(size_t index) {
        T* comp = (T*) components.data() + index;
        comp->~T();
    }
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
    ComponentClassBase* components[MAX_COMPONENTS];
    std::vector<const char*> types;
    std::vector<System> systems;
    std::vector<NativeSystem> nativeSystems;

    uint64_t createSignature(int typeCount, const char* reqTypes[]);
public:
    uint newEntity(jobject jwrapper);
    Entity& getEntity(uint id);
    void removeEntity(uint id);
    uint getTypeID(const char* type);
    template <typename T>
    void putComponent(uint entity, uint type, T& component);
    template <typename T>
    void putComponent(uint entity, const char* type, T& component);
    template <typename T>
    T* getComponent(uint entity, uint type);
    template<typename T>
    T* getComponent(uint entity, const char* type);
    void removeComponent(uint entity, uint type);
    void removeComponent(uint entity, const char* type);
    void addNativeSystem(void (*apply)(Entity&, float), int typeCount, const char* reqTypes[]);
    void addSystem(JNIEnv* env, jobject& jwrapper, jobjectArray& reqTypes);
    void update(JNIEnv* env, jfloat delta);
    void clear(JNIEnv* env);
};

namespace Misery { extern ECS ecs; }

template<typename T>
void ECS::putComponent(uint entity, uint type, T &component) {
    if (components[type] == nullptr) components[type] = new ComponentClass<T>();

    std::vector<uint8_t>& data = components[type]->components;

    size_t index;
    // override previous component of type if exists
    if (entities[entity].signature & 0x1u << type) {
        index = entities[entity].components[type];
    } else { // otherwise allocate new memory
        index = data.size();
        data.resize(index + components[type]->componentSize());
    }

    std::memcpy(&data[index], &component, components[type]->componentSize());

    entities[entity].components[type] = index;
    entities[entity].signature |= (0x1u << type);
}

template<typename T>
void ECS::putComponent(uint entity, const char* type, T &component)
{ putComponent(entity, getTypeID(type), component); }

template<typename T>
T *ECS::getComponent(uint entity, uint type) {
    return (T*) &components[type]->components[entities[entity].components[type]];
}

template<typename T>
T *ECS::getComponent(uint entity, const char* type)
{ return getComponent<T>(entity, getTypeID(type)); }

#endif //MISERY_ECS_H
