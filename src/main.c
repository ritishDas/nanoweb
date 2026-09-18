#include "hashmap.h"
#include "routeTrie.h"
#include "server.h"
#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT "3000"

void testFun(char *s) { printf("%s\n", s); }

int main() {

  TrieNode *routeNode = RouteNodeInit();

  Method m = {.type = GET, .handler = testFun};

  addMethod(routeNode, "/help/rd", &m);

  RouteNodeFree(routeNode);

  server(PORT);
  return EXIT_SUCCESS;
}
