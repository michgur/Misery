//
// Created by micha on 8/30/2020.
//

#include "interaction.h"
#include <algorithm>

InteractionWorld::InteractionWorld(ECS &ecs) : ecs(ecs), comparator{ .ecs = ecs } {
    signature = ~0x0u;
    const char* types[] = { "transform", "aabb" };
    interactableSignature = ecs.createSignature(2, types);
}

#define MATCHES(a, b) (a & b) == b

void InteractionWorld::onPutComponent(uint entity, uint type) {
    if (MATCHES(ecs.getEntity(entity).signature, interactableSignature)) {
        // a new matching entity
        if (0x1u << type & interactableSignature) addEntity(entity);
        // an existing matching entity
        else toUpdate.push_back(entity);
    }
}

void InteractionWorld::onRemoveComponent(uint entity, uint type) {
    // entity no longer matching
    if (0x1u << type & interactableSignature) toRemove.push_back(entity);
    // entity still matching
    else toUpdate.push_back(entity);
}

#define SQ(v) vector3f(v.x * v.x, v.y * v.y, v.z * v.z)
void InteractionWorld::update(float delta) {
    updateEntities();

    std::sort(interactables.begin(), interactables.end(), comparator);

    vector3f center(0), centerSq(0);
    for (uint interactable = 0; interactable < interactables.size(); interactable++) {
        AABB* aabb = ecs.getComponent<AABB>(interactables[interactable].entity, "aabb");
        vector3f c = aabb->getCenter();
        center += c;
        centerSq += SQ(c);

        for (uint other = interactable - 1; other < interactables.size(); other++) {
            AABB* otherBB = ecs.getComponent<AABB>(interactables[other].entity, "aabb");
            if (otherBB->min[comparator.axis] > aabb->max[comparator.axis]) break;

            if (aabb->intersects(*otherBB)) {
                for (auto& active : interactables[interactable].actives)
                    for (auto& passive : interactables[other].passives)
                        if (active == passive) {
                            Interaction* interaction = interactions[interactables[interactable].actives[active]];
                            interaction->apply(interactable, other, delta);
                        }
                for (auto& passive : interactables[interactable].passives)
                    for (auto& active : interactables[other].actives)
                        if (active == passive) {
                            Interaction* interaction = interactions[interactables[interactable].passives[passive]];
                            interaction->apply(interactable, other, delta);
                        }
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

template <typename T>
void swapRemove(std::vector<T> vector, uint index) {
    std::swap(vector[index], vector[vector.size() - 1]);
    vector.pop_back();
}
void InteractionWorld::updateEntities() {
    if (toRemove.empty() && toUpdate.empty()) return;
    for (uint interactable = 0; interactable < interactables.size(); interactable++) {
        bool removed;
        do {
            removed = false;
            for (uint remove = 0; remove < toRemove.size(); remove++)
                if (interactables[interactable].entity == toRemove[remove]) {
                    swapRemove(interactables, interactable);
                    swapRemove(toRemove, remove);
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
                swapRemove(toUpdate, update);
            }
    }
    toRemove.clear();
    toUpdate.clear();
}

void InteractionWorld::addEntity(uint entity) {
    Interactable interactable { .entity = entity };
    for (uint i = 0; i < interactions.size(); i++) findInteractions(interactable, i);
    interactables.push_back(interactable);
}

void InteractionWorld::findInteractions
(InteractionWorld::Interactable &interactable, uint index) {
    Interaction* interaction = interactions[index];
    uint64_t signature = ecs.getEntity(interactable.entity).signature;
    if (MATCHES(signature, interaction->active_signature))
        interactable.actives.push_back(index);
    if (MATCHES(signature, interaction->passive_signature))
        interactable.passives.push_back(index);
}

void InteractionWorld::addInteraction(Interaction *interaction) {
    uint index = interactions.size();
    interactions.push_back(interaction);
    for (auto& interactable : interactables)
        findInteractions(interactable, index);
}
