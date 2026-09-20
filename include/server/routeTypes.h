#ifndef ROUTE_TYPES_H
#define ROUTE_TYPES_H

#include "dataStructure/khash.h"
#include "dataStructure/list.h"
#include <stddef.h>

typedef struct controllerRes {
  char *res;
  size_t reslen;
} ControllerRes;

typedef enum httpMethod { GET, POST, PUT, DELETE, PATCH, QUERY } MethodType;

typedef struct method {
  MethodType type;
  ControllerRes (*handler)();
} Method;

struct routeNode;

KHASH_MAP_INIT_STR(1, struct routeNode *);
KHASH_MAP_INIT_STR(2, char *);

typedef struct routeNode {
  _Bool complete;
  List(char) path;
  Method *method;
  khash_t(1) * children;
  khash_t(1) * dynamicChildren;
} TrieNode;

#endif
