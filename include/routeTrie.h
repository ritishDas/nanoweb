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
  void (*handler)(char *);
} Method;

typedef struct routeNode {
  _Bool complete;
  List(char) path;
  Method *method;
  khash_t(1) * children;
} TrieNode;

typedef Vector(char *) StringVec;
typedef List(char) String;

void pathSeparator(StringVec *vec, String path);

void freeStringVec(StringVec *vec);

void RouteNodeFree(TrieNode *tn);

void addMethod(TrieNode *nanoweb, char *path, Method *method);

TrieNode *RouteNodeInit(void);
#endif
