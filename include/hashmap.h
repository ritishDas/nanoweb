#include "khash.h"
#include "routeTrie.h"

khash_t(1) * newMap();

void insertMap(khash_t(1) * dictionary, char *key, TrieNode *val);

void destroyMap(khash_t(1) * dictionary);

_Bool checkKey(khash_t(1) * dictionary, char *key);

