//
// Created by micha on 8/30/2020.
//

#ifndef MISERY_INTERACTION_H
#define MISERY_INTERACTION_H

#include "Transform.h"
#include <assimp/vector3.inl>

struct AABB {
    union {
        struct {
            vector3f min = vector3f(0);
            vector3f max = vector3f(0);
        };
        float data[6];
    };

    inline AABB() {}

    inline vector3f getCenter() { return max + min / 2.0f; }
    inline bool intersects(AABB& other) {
        return !(
            other.min.x > max.x || other.min.y > max.y || other.min.z > max.z ||
            other.max.x > min.x || other.max.y > min.y || other.max.z > min.z
        );
    }
};

struct Interaction {
    uint64_t active_signature;
    uint64_t passive_signature;

    bool native;
    union Function {
        struct {
            jobject jwrapper;
            jmethodID invoke;
        } jni;
        void (*native)(uint active, uint passive, float delta);
    } *function;
};

class InteractionWorld : public ECSListener {
    struct Interactable {
        uint entity;
        std::vector<uint> actives;
        std::vector<uint> passives;
    };
    uint64_t interactableSignature;

    ECS& ecs;
    std::vector<Interactable> interactables;
    std::vector<uint> toRemove;
    std::vector<uint> toUpdate;
    std::vector<Interaction> interactions;

    struct {
        ECS& ecs;
        int axis = 0;
        bool operator()(Interactable& a, Interactable& b) {
            float aMin = ecs.getComponent<AABB>(a.entity, "aabb")->min[axis];
            float bMin = ecs.getComponent<AABB>(b.entity, "aabb")->min[axis];
            return aMin < bMin;
        }
    } comparator;

    void addEntity(uint entity);
    void findInteractions(Interactable& interactable, uint index);
    void interact(JNIEnv* env, float delta, Interactable& active, Interactable& passive);
    void updateEntities();
public:
    InteractionWorld(ECS& ecs);

    void addInteraction(uint activeTypeCount, const char* activeTypes[],
            uint passiveTypeCount, const char* passiveTypes[], void (*apply)(uint, uint, float));
    void addInteraction(JNIEnv* env, jobject& jwrapper,
            jobjectArray& activeTypes, jobjectArray& passiveTypes);

    void onPutComponent(uint entity, uint type);
    void onRemoveComponent(uint entity, uint type);

    void update(JNIEnv* env, float delta);
    void clear(JNIEnv* env);
};

namespace Misery { extern InteractionWorld interactions; }

#endif //MISERY_INTERACTION_H
