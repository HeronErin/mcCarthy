#pragma once


// This file contains an ultra simplified version of the typical
// hashmap, but instead of using hashes, unix file descripters.
// Each file descriptor is unique, and once one become invalid
// you must remove it otherwise you will confuse one player with
// another. 


#include <stdlib.h>
#include <string.h>
struct _MapElement{
    int descriptor;
    void* element;
    struct _MapElement* collisionBuddy;
};
typedef struct _MapElement MapElement;

typedef struct {
    int maxElements;
    MapElement elements[];
} DescriptorMap;

DescriptorMap* makeDescriptorMap(int maxElements){
    DescriptorMap* map = calloc(sizeof(DescriptorMap) + maxElements * sizeof(MapElement),1);
    map->maxElements = maxElements;
    return map;
}
void** lookupElement(DescriptorMap* map, int descriptor){
    int index = descriptor % map->maxElements;
    MapElement* e = &map->elements[index];
    if (e->descriptor == 0)
        return NULL;
    CHECK_ELEMENT:
    if (e->descriptor == descriptor)
        return &e->element;
    if (e->collisionBuddy != NULL){
        e = e->collisionBuddy;
        goto CHECK_ELEMENT;
    }
    return NULL;
}
void* removeElement(DescriptorMap* map, int descriptor){
    int index = descriptor % map->maxElements;
    MapElement* e = &map->elements[index];
    MapElement* cameFromCollision = NULL;
    void* rdata;
    if (e->descriptor == 0)
        return NULL;
    CHECK_ELEMENT:
    if (e->descriptor == descriptor){
        if (e->collisionBuddy != NULL){
            void* oldLoc = e->collisionBuddy;
            rdata = e->element;
            memcpy(&map->elements[index], e->collisionBuddy, sizeof(MapElement));
            free(oldLoc);
            return rdata;
        }
        rdata = e->element;
        memset(e, 0, sizeof(MapElement));
        if (cameFromCollision)
            free(cameFromCollision->collisionBuddy);
        return rdata;
    }
    if (e->collisionBuddy != NULL){
        cameFromCollision = e;
        e = e->collisionBuddy;
        goto CHECK_ELEMENT;
    }
    return NULL;
}
int setElement(DescriptorMap* map, int descriptor, void* data){
    int index = descriptor % map->maxElements;
    MapElement* e = &map->elements[index];
    TEST_AND_SET_DESC:
    if (e->descriptor == 0 || e->descriptor == descriptor){
        e->descriptor = descriptor;
        e->element = data;
        return 0;
    }
    if (e->collisionBuddy == NULL){
        e->collisionBuddy = calloc(1, sizeof(MapElement));
        if (e->collisionBuddy == NULL) return -1;
    }
    e = e->collisionBuddy;
    goto TEST_AND_SET_DESC;
}
void freeMap(DescriptorMap* map){
    MapElement* e;
    MapElement* cameFromCollision;
    int mod = map->maxElements;
    for (int i = 0; i < map->maxElements; i++){
        cameFromCollision = NULL;
        e = &map->elements[i];
        FREE_ELEMENT:
        if (e->element) free(e->element);
        if (e->collisionBuddy){
            cameFromCollision = e;
            e = e->collisionBuddy;
            free(cameFromCollision);
            goto FREE_ELEMENT;
        }
    }
    free(map);
}