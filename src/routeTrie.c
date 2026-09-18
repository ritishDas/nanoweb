#include "routeTrie.h"
#include "hashmap.h"
#include "khash.h"
#include "vector.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *copyChars(const char *data, size_t len) {
  char *copy = malloc(len + 1);
  if (!copy)
    return NULL;

  memcpy(copy, data, len);
  copy[len] = '\0';
  return copy;
}

static int setNodePath(TrieNode *node, const char *data, size_t len) {
  char *copy = copyChars(data, len);
  if (!copy)
    return 0;

  free(node->path.data);
  node->path.data = copy;
  node->path.size = len;
  return 1;
}

TrieNode *RouteNodeInit(void) {
  TrieNode *newNode = malloc(sizeof(TrieNode));
  if (!newNode)
    return NULL;

  newNode->path.data = NULL;
  newNode->path.size = 0;
  newNode->complete = 0;
  newNode->children = newMap();
  newNode->method = NULL;

  if (!newNode->children ||
      !setNodePath(newNode, "nanoweb", strlen("nanoweb"))) {
    destroyMap(newNode->children);
    free(newNode);
    return NULL;
  }

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
  free(tn->path.data);
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

void addMethod(TrieNode *nanoweb, char *path, Method *method) {
  if (!nanoweb || !path || !method)
    return;

  StringVec test;
  VECTOR_INIT(&test);

  String p = {.data = path, .size = strlen(path)};

  pathSeparator(&test, p);

  TrieNode *temp = nanoweb;
  for (size_t i = 0; i < test.size; i++) {
    printf("%s\n", test.data[i]);

    TrieNode *temp2 = checkKey(temp->children, test.data[i]);
    if (!temp2) {

      TrieNode *newNode = RouteNodeInit();
      if (!newNode ||
          !setNodePath(newNode, test.data[i], strlen(test.data[i]))) {
        RouteNodeFree(newNode);
        break;
      }

      insertMap(temp->children, test.data[i], newNode);
      temp = newNode;
    } else {
      temp = temp2;
    }

    if (i == test.size - 1) {
      Method *met = malloc(sizeof(Method));
      if (!met)
        break;

      *met = *method;
      free(temp->method);
      temp->method = met;
      temp->complete = 1;
    }
  }

  freeStringVec(&test);
  VECTOR_FREE(&test);
}
