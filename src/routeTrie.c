#include "routeTrie.h"
#include "hashmap.h"
#include "khash.h"
#include "vector.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

TrieNode *RouteNodeInit(void) {
  TrieNode *newNode = malloc(sizeof(TrieNode));
  if (!newNode)
    return NULL;

  newNode->path.data = "nanoweb";
  newNode->path.size = strlen("nanoweb");
  newNode->children = newMap();
  newNode->method = NULL;

  return newNode;
}

void RouteNodeFree(TrieNode *tn) {
  if (!tn)
    return;

  for (khiter_t k = kh_begin(tn->children); k != kh_end(tn->children); ++k) {

    if (kh_exist(tn->children, k)) {
      TrieNode *child = kh_value(tn->children, k);
      RouteNodeFree(child);
    }
  }

  destroyMap(tn->children);
  free(tn->method);
  free(tn);
}

void pathSeparator(StringVec *vec, String path) {
  size_t start = 0;

  while (start < path.size && path.data[start] == '/') {
    start++;
  }

  for (size_t i = start; i <= path.size; i++) {
    if (i == path.size || path.data[i] == '/') {
      size_t len = i - start;
      if (len > 0) {
        char *segment = malloc(len + 1);
        if (!segment)
          return;

        memcpy(segment, path.data + start, len);
        segment[len] = '\0';
        VECTOR_PUSH(vec, segment);
      }
      start = i + 1;
    }
  }
}

void freeStringVec(StringVec *vec) {
  for (size_t i = 0; i < vec->size; i++) {
    free(vec->data[i]);
  }
}

// void addMethod(TrieNode *nanoweb, char *path, Method method) {
//   if (!nanoweb || !path)
//     return;
//
//   StringVec test;
//   VECTOR_INIT(&test);
//
//   String p = {.data = path, .size = strlen(path)};
//
//   pathSeparator(&test, p);
//
//   for (size_t i = 0; i < test.size; i++) {
//     printf("%s\n", test.data[i]);
//
//     // if (kh_exist(nanoweb->children, test.data[i]))
//
//     // kh_exist(nanoweb->children)
//     // TODO: Insert segment into TrieNode
//     // free(test.data[i]); // Remember to manage segment lifetimes
//   }
// }
