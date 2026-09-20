#ifndef ROUTETRIE_H
#define ROUTETRIE_H

#include "list.h"
#include "routeTypes.h"
#include "vector.h"

typedef Vector(char *) StringVec;
typedef List(char) String;

void pathSeparator(StringVec *vec, String path);

void freeStringVec(StringVec *vec);

void RouteNodeFree(TrieNode *tn);

void addMethod(TrieNode *nanoweb, char *path, Method *method);

TrieNode *RouteNodeInit(void);

struct serverRequest;

void routeMatcher(struct serverRequest *userReq, TrieNode *nanoweb, String path,
                  int receiverFd);

#endif
