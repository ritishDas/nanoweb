
#include "routeTrie.h"
#include "list.h"
#include "vector.h"
#include <stdlib.h>

TrieNode *RouteNodeInit() {
  TrieNode *new = malloc(sizeof(TrieNode));

  new->path.data = "nanoweb";
  new->path.size = 8;
  new->children = nullptr;
  new->method = nullptr;

  return new;
}

// void addMethod(char *path, Method method) {}

// void pathSeparator(Vector(char *) vec, List(char) path) {
//   int first = 0;
//
//   for (int i = 1; i < path.size; i++) {
//
//     if (path[i] == '/')
//   }
// }
