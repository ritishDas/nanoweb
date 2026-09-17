#ifndef ROUTETRIE_H
#define ROUTETRIE_H

#include "list.h"
#include "vector.h"

typedef enum httpMethod { GET, POST, PUT, DELETE, PATCH, QUERY } MethodType;

typedef struct method {
  MethodType type;
} Method;

typedef struct routeNode {
  List(char) path;
  Method *method;
  // controllers here
  Vector(struct routeNode *) * children;
} TrieNode;

TrieNode *RouteNodeInit();

#endif
