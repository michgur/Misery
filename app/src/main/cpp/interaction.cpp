//
// Created by micha on 8/30/2020.
//

#include "interaction.h"
#include "physics.h"
#include <algorithm>
#include <limits>

namespace Misery { InteractionWorld interactions(ecs); }

InteractionWorld::InteractionWorld(ECS &ecs) : ecs(ecs), comparator{ .ecs = ecs } {
    signature = ~0x0u;
    const char* types[] = { "transform", "aabb" };
    interactableSignature = ecs.createSignature(2, types);
    ecs.addListener(this);

    // todo does not belong here
    const char* motion_types[] = { "motion", "transform" };
    ecs.addSystem(motionSystem, 2, motion_types);
}

#define MATCHES(a, b) ((a & b) == b)

void InteractionWorld::onPutComponent(uint entity, uint type) {
    if (MATCHES(ecs.getEntity(entity).signature, interactableSignature)) {
        // a new matching entity
        if (0x1u << type & interactableSignature) addEntity(entity);
        // an existing matching entity
        else toUpdate.push_back(entity);
    }
}

void InteractionWorld::onRemoveComponent(uint entity, uint type) {
    if (MATCHES(ecs.getEntity(entity).signature, interactableSignature)) {
        // entity no longer matching
        if (0x1u << type & interactableSignature) toRemove.push_back(entity);
        // entity still matching
        else toUpdate.push_back(entity);
    }
}

void InteractionWorld::interact(JNIEnv* env, float delta, Interactable& active, Interactable& passive) {
    for (auto& activeInteraction : active.actives)
        for (auto& passiveInteraction : passive.passives)
            if (activeInteraction == passiveInteraction) {
                Interaction& interaction = interactions[activeInteraction];
                if (interaction.native)
                    interaction.function->native(active.entity, passive.entity, delta);
                else {
                    env->CallVoidMethod(
                        interaction.function->jni.jwrapper,
                        interaction.function->jni.invoke,
                        ecs.getEntity(active.entity).jwrapper,
                        ecs.getEntity(passive.entity).jwrapper,
                        delta
                    );
                    JNI_CATCH_EXCEPTION("java exception occurred when trying to perform interaction %p on entities %i, %i",
                             &interaction, active.entity, passive.entity);
                }
            }
}

#define SQ(v) vector3f(v.x * v.x, v.y * v.y, v.z * v.z)
void InteractionWorld::update(JNIEnv* env, float delta) {
    updateEntities();

    for (auto& interactable : interactables) {
        uint entity = interactable.entity;
        matrix4f transform = ecs.getComponent<Transform>(entity, "transform")->toMatrix();
        AABB aabb = ecs.getComponent<AABB>(entity, "aabb")->transform(transform);
        std::memcpy(ecs.getComponent<AABB>(entity, "_taabb")->data, aabb.data, sizeof(AABB));
    }
    std::sort(interactables.begin(), interactables.end(), comparator);

    vector3f center(0), centerSq(0);
    for (uint i = 0; i < interactables.size(); i++) {
        Interactable& interactable = interactables[i];
        // make sure that the entity was not deleted mid-run by one of the interactions
        if (!ecs.entityExists(interactable.entity)) continue;

        AABB* aabb = ecs.getComponent<AABB>(interactable.entity, "_taabb");
        vector3f c = aabb->getCenter();
        center += c;
        centerSq += SQ(c);

        for (uint j = i + 1; j < interactables.size(); j++) {
            Interactable& other = interactables[j];
            // make sure that the entity was not deleted mid-run by one of the interactions
            if (!ecs.entityExists(other.entity)) continue;

            AABB* otherBB = ecs.getComponent<AABB>(other.entity, "_taabb");
            if (otherBB->min[comparator.axis] > aabb->max[comparator.axis]) break;

            if (aabb->intersects(*otherBB)) {
                interact(env, delta, interactable, other);
                interact(env, delta, other, interactable);
            }
        }
    }
    center /= interactables.size();
    centerSq /= interactables.size();

    vector3f variance = centerSq - SQ(center);
    comparator.axis = 0;
    float max = variance.x;
    if (max < variance.y) { comparator.axis = 1; max = variance.y; }
    if (max < variance.z) comparator.axis = 2;
#undef SQ
}

#define SWAP_REMOVE(v, i) std::swap(v[i], v[v.size() - 1]); v.pop_back()
void InteractionWorld::updateEntities() {
    if (toRemove.empty() && toUpdate.empty()) return;
    for (uint interactable = 0; interactable < interactables.size(); interactable++) {
        bool removed;
        do {
            removed = false;
            for (uint remove = 0; remove < toRemove.size(); remove++)
                if (interactables[interactable].entity == toRemove[remove]) {
                    SWAP_REMOVE(interactables, interactable);
                    SWAP_REMOVE(toRemove, remove);
                    if (toRemove.empty() && toUpdate.empty()) return;

                    removed = true;
                    break;
                }
        } while (removed);

        for (uint update : toUpdate)
            if (interactables[interactable].entity == update) {
                interactables[interactable].actives.clear();
                interactables[interactable].passives.clear();
                for (uint i = 0; i < interactions.size(); i++)
                    findInteractions(interactables[interactable], i);
                SWAP_REMOVE(toUpdate, update);
                break;
            }
    }
    toRemove.clear();
    toUpdate.clear();
#undef SWAP_REMOVE
}

void InteractionWorld::addEntity(uint entity) {
    Interactable interactable { .entity = entity };
    for (uint i = 0; i < interactions.size(); i++) findInteractions(interactable, i);
    interactables.push_back(interactable);

    matrix4f transform = ecs.getComponent<Transform>(entity, "transform")->toMatrix();
    AABB aabb = ecs.getComponent<AABB>(entity, "aabb")->transform(transform);
    ecs.putComponent(entity, "_taabb", aabb);
}

void InteractionWorld::findInteractions
(InteractionWorld::Interactable &interactable, uint index) {
    Interaction& interaction = interactions[index];
    uint64_t signature = ecs.getEntity(interactable.entity).signature;
    if (MATCHES(signature, interaction.active_signature))
        interactable.actives.push_back(index);
    if (MATCHES(signature, interaction.passive_signature))
        interactable.passives.push_back(index);
}

void InteractionWorld::addInteraction(uint activeTypeCount, const char **activeTypes,
              uint passiveTypeCount, const char **passiveTypes, void (*apply)(uint, uint, float)) {
    uint index = interactions.size();
    interactions.push_back(Interaction {
            .active_signature = ecs.createSignature(activeTypeCount, activeTypes),
            .passive_signature = ecs.createSignature(passiveTypeCount, passiveTypes),
            .native = true,
            .function = new Interaction::Function { .native = apply }
    });
    for (auto& interactable : interactables)
        findInteractions(interactable, index);
}

void InteractionWorld::addInteraction(JNIEnv *env, jobject &jwrapper, jobjectArray &activeTypes,
                                      jobjectArray &passiveTypes) {
    uint64_t active_signature = ecs.createSignature(env, activeTypes);
    uint64_t passive_signature = ecs.createSignature(env, passiveTypes);

    jclass jinteractionclass = env->GetObjectClass(jwrapper);
    jmethodID jinteractioninvoke = env->GetMethodID(jinteractionclass,
            "invoke", "(Lcom/klmn/misery/update/Entity;Lcom/klmn/misery/update/Entity;F)V");

    uint index = interactions.size();
    interactions.push_back(Interaction {
        .active_signature = active_signature,
        .passive_signature = passive_signature,
        .native = false,
        .function = new Interaction::Function {
            .jni.jwrapper = env->NewGlobalRef(jwrapper),
            .jni.invoke = jinteractioninvoke
        }
    });
    for (auto& interactable : interactables)
        findInteractions(interactable, index);
}

void InteractionWorld::clear(JNIEnv *env) {
    for (auto& interaction : interactions) {
        if (interaction.native)
            env->DeleteGlobalRef(interaction.function->jni.jwrapper);
        delete interaction.function;
    }
}

bool InteractionWorld::Comparator::operator()(InteractionWorld::Interactable &a,
                                              InteractionWorld::Interactable &b) {
    float aMin = ecs.getComponent<AABB>(a.entity, "_taabb")->min[axis];
    float bMin = ecs.getComponent<AABB>(b.entity, "_taabb")->min[axis];
    return aMin < bMin;
}
