# Nanoweb 

This is a web server written in pure c. Currently it's in the initial development phase.

## Work Done

* A web server that listens on port 3000 (configurable from `./src/main.c`). 
* Currently supports synchronous request handling.
* Setup pico http parser to parse http request and send response.

## Getting Started 

* Clone the repository.
* Build the project with `make`.
* Run with ./bin/app.

## API Reference

```c
#define PORT "3000"

  TrieNode *routeNode = RouteNodeInit();
  Method m = {.type = GET, .handler = testFun};
  addMethod(routeNode, "/help/rd", &m);
  RouteNodeFree(routeNode);

// routeMatcher(routeNode, "/help/rd", receiverFd);
// routeMatcher(routeNode, "/help/:id", receiverFd);

  server(PORT);
```

* `server(PORT)` : Starts the http server on the port number.
* `addMethod(routeNode, "/help/rd", &m);` : This add a new route with controllers for each method.
* `routeMatcher(routeNode, "/help/rd", receiverFd);` : This will match path from incoming request and run the appropriate controller.
* Dynamic route is now supported.
