#pragma once
#include "dataStructure/khash.h"
#include "server/routeTrie.h"

khash_t(1) * newMap();

void insertMap(khash_t(1) * dictionary, char *key, TrieNode *val);

void destroyMap(khash_t(1) * dictionary);

TrieNode *checkKey(khash_t(1) * dictionary, char *key);

khash_t(2) * newMap2();

void insertCharMap(khash_t(2) * dictionary, const char *key, const char *value);

void destroyMap2(khash_t(2) * dictionary);

khash_t(2) * copyMap2(khash_t(2) * dictionary);
