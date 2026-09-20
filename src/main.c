#include "routeTrie.h"
#include "server.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#define PORT "3000"

ControllerRes helloController() {

  char *response_buffer = malloc(sizeof(char) * 1024);
  size_t response_len = sizeof(char) * 1024;

  const char *html_body = "<h1>Hello, World!</h1><p>Welcome to the server.</p>";

  newServerResponse(response_buffer, &response_len, 200, "text/html",
                    html_body);

  struct controllerRes response = {.res = response_buffer,
                                   .reslen = response_len};
  return response;
}

int main() {

  TrieNode *routeNode = RouteNodeInit();
  Method m = {.type = GET, .handler = helloController};
  addMethod(routeNode, "/help/:rd", &m);
  // addMethod(routeNode, "/help/rd", &m);
  // routeMatcher(routeNode, "/help/rd");

  server(PORT, routeNode);

  RouteNodeFree(routeNode);
  return EXIT_SUCCESS;
}
