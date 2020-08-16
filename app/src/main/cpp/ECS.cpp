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

void ECS::addComponent(uint entity, std::string& type, void *component) { addComponent(entity, getTypeID(type), component); }
void ECS::addComponent(uint entity, uint type, void *component) {
    entities[entity].components[type] = components[type].size();
    components[type].push_back(component);

    entities[entity].signature |= (0x1u << type);
}

void* ECS::getComponent(uint entity, std::string& type) { return getComponent(entity, getTypeID(type)); }
void* ECS::getComponent(uint entity, uint type) {
    uint component = entities[entity].components[type];
    return components[type][component];
}

uint64_t ECS::createSignature(uint64_t signature, std::string& arg) { return signature | 0x1u << getTypeID(arg); }

template<typename ...T>
uint64_t ECS::createSignature(uint64_t signature, std::string& arg, T&... args) {
    signature = createSignature(signature, arg);
    return createSignature(signature, args...);
}

void ECS::newSystem(void (*apply)(Entity &), std::string& reqType0, std::string& reqTypes, ...)
{ systems.push_back(System { createSignature(0, reqType0, reqTypes), apply });}

ECS &ECS::getInstance() {
    static ECS instance;
    return instance;
}
