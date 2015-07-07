/*
=====================================================================
hashmap.cpp: Implementation of a simple hashmap
author: Chris-Andre Leimeister
====================================================================
 */

#include "hashmap.h"

int hashmap::getHash(unsigned long long key) {
    return (int) (key >> (64 - n));
}

void hashmap::add(unsigned long long key) {
    int h = this->getHash(key);
    // handle collisions
    while (h < map->size()) {
        if ((*map)[h].first == key) {
            (*map)[h].second++;
            return;
        } else if ((*map)[h].second == 0) {
            (*map)[h].first = key;
            (*map)[h].second = 1;
            return;
        }
        h++;
    }
    h = 0;
    while (h < map->size()) {
        if ((*map)[h].first == key) {
            (*map)[h].second++;
            return;
        } else if ((*map)[h].second == 0) {
            (*map)[h].first = key;
            (*map)[h].second = 1;
            return;
        }
        h++;
    }
    while (true)
        cout << "An error occured!: k too large!" << endl;
}

int hashmap::getValue(unsigned long long key) {
    int h = this->getHash(key);
    while (h < map->size()) {
        if ((*map)[h].first == key)
            return (*map)[h].second;
        else if ((*map)[h].second == 0)
            return 0;
        h++;
    }
    h = 0;
    while (h < map->size()) {
        if ((*map)[h].first == key)
            return (*map)[h].second;
        else if ((*map)[h].second == 0)
            return 0;
        h++;
    }
    return 0;
}
