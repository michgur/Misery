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
    virtual void* data() = 0;
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

// TODO: better memory management (store component values in one place, override unused components)
//       consider splitting to native / java components and systems
//       add basic component classes- think about the desired interface for the kotlin code
// the advantage of having the ECS in native code is storage efficiency,
//      that's the main principle to consider for this design
// the second thing to consider- minimizing JNI traffic. things like the transform component
//      may be used excessively in both native code and java
class ECS {
    std::vector<Entity> entities;
    std::vector<jobject> components[MAX_COMPONENTS];
    std::map<uint, NativeComponentBase*> nativeComponents;
    std::vector<const char*> types;
    std::vector<System> systems;
    std::vector<NativeSystem> nativeSystems;

    uint64_t createSignature(int typeCount, const char* reqTypes[]);
public:
    uint newEntity(jobject jwrapper);
    Entity& getEntity(uint id);
    uint getTypeID(const char* type);
    template <typename T>
    void addNativeComponent(uint entity, uint type, T& component);
    template <typename T>
    void addNativeComponent(uint entity, const char* type, T& component);
    template <typename T>
    T* getNativeComponent(uint entity, uint type);
    template<typename T>
    T* getNativeComponent(uint entity, const char* type);
    bool removeNativeComponent(uint entity, uint type);
    bool removeNativeComponent(uint entity, const char* type);
    void addComponent(uint entity, uint type, jobject& component);
    void addComponent(uint entity, const char* type, jobject& component);
    jobject* getComponent(uint entity, uint type);
    jobject* getComponent(uint entity, const char* type);
    void addNativeSystem(void (*apply)(Entity&, float), int typeCount, const char* reqTypes[]);
    void addSystem(JNIEnv* env, jobject& jwrapper, jobjectArray& reqTypes);
    void update(JNIEnv* env, jfloat delta);
    void clear(JNIEnv* env);
};

namespace Misery { extern ECS ecs; }

template<typename T>
void ECS::addNativeComponent(uint entity, uint type, T &component) {
    auto it = nativeComponents.find(type);
    if (it == nativeComponents.end()) nativeComponents[type] = new NativeComponentClass<T>();
    auto* data = (std::vector<T>*) nativeComponents[type]->data();
    entities[entity].components[type] = data->size();
    data->push_back(component);

    entities[entity].signature |= (0x1u << type);
}

template<typename T>
void ECS::addNativeComponent(uint entity, const char* type, T &component)
{ addNativeComponent(entity, getTypeID(type), component); }

template<typename T>
T *ECS::getNativeComponent(uint entity, uint type) {
    uint id = entities[entity].components[type];
    return ((std::vector<T>*) nativeComponents[type]->data())->data() + id;
}

template<typename T>
T *ECS::getNativeComponent(uint entity, const char* type)
{ return getNativeComponent<T>(entity, getTypeID(type)); }

#endif //MISERY_ECS_H
