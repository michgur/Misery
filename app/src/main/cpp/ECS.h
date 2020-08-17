//
// Created by micha on 8/15/2020.
//

#ifndef MISERY_ECS_H
#define MISERY_ECS_H

#define MAX_COMPONENTS 64

#include <vector>
#include <map>
#include <string>
#include <jni.h>

struct Entity {
    uint id;
    uint64_t signature = 0;
    std::map<uint, uint> components;
};

struct NativeComponentBase { virtual void* data() { return nullptr; } };
template <typename T>
struct NativeComponentClass : NativeComponentBase {
    std::vector<T> components;
    void* data() { return &components; }
};

struct System {
    uint64_t signature;
    jobject apply;
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
    std::map<uint, NativeComponentBase> nativeComponents;
    std::vector<std::string> types;
    std::vector<System> systems;
    std::vector<NativeSystem> nativeSystems;

    template<typename... T>
    uint64_t createSignature(uint64_t signature, std::string &arg, T &... args);
    uint64_t createSignature(uint64_t signature, std::string &arg);
public:
    uint newEntity();
    uint getTypeID(std::string& type);
    template <typename T>
    void addNativeComponent(uint entity, uint type, T& component);
    template <typename T>
    void addNativeComponent(uint entity, std::string& type, T& component);
    template <typename T>
    T* getNativeComponent(uint entity, uint type);
    template<typename T>
    T *getNativeComponent(uint entity, std::string& type);
    void addComponent(uint entity, uint type, jobject& component);
    void addComponent(uint entity, std::string& type, jobject& component);
    jobject* getComponent(uint entity, uint type);
    jobject* getComponent(uint entity, std::string& type);
    void addNativeSystem(void (*apply)(Entity&, float), std::string &reqType0, std::string &reqTypes, ...);
    void addSystem(JNIEnv* env, jobject& apply, jobjectArray& reqTypes);
    void update(JNIEnv* env, jfloat delta);

    static ECS& getInstance();
};

#endif //MISERY_ECS_H
