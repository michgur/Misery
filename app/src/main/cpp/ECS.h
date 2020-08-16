//
// Created by micha on 8/15/2020.
//

#ifndef MISERY_ECS_H
#define MISERY_ECS_H

#define MAX_COMPONENTS 64

#include <vector>
#include <map>
#include <string>

struct Entity {
    uint id;
    uint64_t signature = 0;
    std::map<uint, uint> components;
};

struct System {
    uint64_t signature;
    void (*apply)(Entity&);
};

// TODO: better memory management (store component values in one place, override unused components)
//       consider splitting to native / java components and systems
//       add basic component classes- think about the desired interface for the kotlin code
// the advantage of having the ECS in native code is storage efficiency,
// that's the main principle to consider for this design
class ECS {
    std::vector<Entity> entities;
    std::vector<void*> components[MAX_COMPONENTS];
    std::vector<std::string> types;
    std::vector<System> systems;

    template<typename... T>
    uint64_t createSignature(uint64_t signature, std::string &arg, T &... args);
    uint64_t createSignature(uint64_t signature, std::string &arg);
public:
    uint newEntity();
    uint getTypeID(std::string& type);
    void addComponent(uint entity, uint type, void* component);
    void addComponent(uint entity, std::string& type, void* component);
    void* getComponent(uint entity, uint type);
    void* getComponent(uint entity, std::string& type);
    void newSystem(void (*apply)(Entity &), std::string &reqType0, std::string &reqTypes, ...);

    static ECS& getInstance();
};

#endif //MISERY_ECS_H
