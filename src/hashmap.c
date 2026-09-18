#include "khash.h"
#include "routeTrie.h"

khash_t(1) * newMap() { return kh_init(1); }

void insertMap(khash_t(1) * dictionary, char *key, TrieNode *val) {
  int kStatus;
  khiter_t kIter;

  kIter = kh_put(1, dictionary, key, &kStatus);
  if (kStatus) {
    kh_value(dictionary, kIter) = val;
  }
}

void destroyMap(khash_t(1) * dictionary) { kh_destroy(1, dictionary); }

_Bool checkKey(khash_t(1) * dictionary, char *key) {
  khiter_t kIter;
  kIter = kh_get(1, dictionary, key);
  return kh_exist(dictionary, kIter);
}
