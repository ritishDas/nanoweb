#ifndef ROUTETRIE_H
#define ROUTETRIE_H

#include "khash.h"

struct routeNode;
KHASH_MAP_INIT_STR(1, struct routeNode *);

#include "khash.h"
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
  khash_t(1) * children;
} TrieNode;

typedef Vector(char *) StringVec;
typedef List(char) String;

// TrieNode *RouteNodeInit();

void pathSeparator(StringVec *vec, String path);

void freeStringVec(StringVec *vec);

void RouteNodeFree(TrieNode *tn);

TrieNode *RouteNodeInit(void);
#endif
