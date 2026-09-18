#include "khash.h"
#include "routeTrie.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

khash_t(1) * newMap() { return kh_init(1); }

static char *copyString(const char *s) {
  size_t len = strlen(s) + 1;
  char *copy = malloc(len);
  if (!copy)
    return NULL;

  memcpy(copy, s, len);
  return copy;
}

void insertMap(khash_t(1) * dictionary, char *key, TrieNode *val) {
  if (!dictionary || !key)
    return;

  int kStatus;
  khiter_t kIter;

  char *ownedKey = copyString(key);
  if (!ownedKey)
    return;

  kIter = kh_put(1, dictionary, ownedKey, &kStatus);
  if (kStatus > 0) {
    kh_value(dictionary, kIter) = val;
  } else {
    free(ownedKey);
  }

  printf("inserted %s", key);
}

void destroyMap(khash_t(1) * dictionary) {
  if (!dictionary)
    return;

  for (khiter_t k = kh_begin(dictionary); k != kh_end(dictionary); ++k) {
    if (kh_exist(dictionary, k)) {
      free((char *)kh_key(dictionary, k));
    }
  }

  kh_destroy(1, dictionary);
}

TrieNode *checkKey(khash_t(1) * dictionary, char *key) {
  if (!dictionary || kh_size(dictionary) == 0)
    return NULL;

  khiter_t kIter = kh_get(1, dictionary, key);

  if (kIter != kh_end(dictionary))
    return kh_value(dictionary, kIter);

  return NULL;
}
