#include "hashmap.h"
#include "routeTrie.h"
#include "server.h"
#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT "3000"

int main() {

  StringVec test;
  VECTOR_INIT(&test);
  String path = {.data = "/hi/:id/das", .size = sizeof("/hi/:id/das")};
  pathSeparator(&test, path);

  TrieNode *routeNode = RouteNodeInit();

  for (size_t i = 0; i < test.size; i++) {
    printf("%s\n", test.data[i]);

    TrieNode *new = RouteNodeInit();

    Method *met = malloc(sizeof(Method));
    met->type = GET;
    new->method = met;

    insertMap(routeNode->children, test.data[i], new);

    routeNode = new;
  }

  RouteNodeFree(routeNode);
  freeStringVec(&test);

  server(PORT);
  return EXIT_SUCCESS;
}
