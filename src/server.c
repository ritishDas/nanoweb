#define _POSIX_C_SOURCE 202405L

#include "server.h"
#include "hashmap.h"
#include "picohttpparser.h"
#include "routeTrie.h"
#include <netdb.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static volatile sig_atomic_t running = 1;

void handle_sigint(int sig) {
  (void)sig;
  running = 0;
}

static const char *get_status_text(int status_code) {
  switch (status_code) {
  case 200:
    return "OK";
  case 201:
    return "Created";
  case 204:
    return "No Content";
  case 400:
    return "Bad Request";
  case 403:
    return "Forbidden";
  case 404:
    return "Not Found";
  case 405:
    return "Method Not Allowed";
  case 500:
    return "Internal Server Error";
  default:
    return "Unknown";
  }
}

void newServerResponse(char *buffer, size_t *bufferSize, int status_code,
                       const char *content_type, const char *body) {

  const char *safe_body = body ? body : "";
  const char *safe_type = content_type ? content_type : "text/plain";
  size_t body_len = strlen(safe_body);
  const char *status_text = get_status_text(status_code);

  int written =
      snprintf(buffer, *bufferSize,
               "HTTP/1.1 %d %s\r\n"
               "Content-Type: %s\r\n"
               "Content-Length: %zu\r\n"
               "Connection: close\r\n"
               "\r\n"
               "%s",
               status_code, status_text, safe_type, body_len, safe_body);

  if (written < 0 || (size_t)written >= *bufferSize) {
    perror("Buffer Too Small");
    exit(1);
  }

  *bufferSize = (size_t)written;
}

Request *newServerRequest(const char *method, size_t method_len,
                          const char *path, size_t path_len,
                          struct phr_header *headers, size_t header_len) {

  Request *new = malloc(sizeof(Request));

  new->path.data = path;
  new->path.size = path_len;

  new->method.data = method;
  new->method.size = method_len;

  new->header.data = headers;
  new->header.size = header_len;

  new->params = nullptr;

  return new;
}

void server(char *port, TrieNode *routeNode) {

  struct addrinfo hint = {.ai_family = AF_UNSPEC,
                          .ai_socktype = SOCK_STREAM,
                          .ai_flags = AI_PASSIVE};
  struct addrinfo *res = NULL;

  if (getaddrinfo(NULL, port, &hint, &res) != 0) {
    perror("getaddrinfo");
    exit(1);
  }

  int serverFd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  if (serverFd == -1) {
    perror("socket");
    freeaddrinfo(res);
    exit(2);
  }

  int opt = 1;
  setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  if (bind(serverFd, res->ai_addr, res->ai_addrlen) == -1) {
    perror("bind");
    close(serverFd);
    freeaddrinfo(res);
    exit(3);
  }
  freeaddrinfo(res);

  if (listen(serverFd, 10) == -1) {
    perror("listen");
    close(serverFd);
    exit(4);
  }

  printf("Server listening on port %s 🚀\n", port);

  struct sigaction sa = {0};
  sa.sa_handler = handle_sigint;
  sigemptyset(&sa.sa_mask);
  sigaction(SIGINT, &sa, NULL);

  while (running) {
    struct sockaddr_storage theirAddr;
    socklen_t size = sizeof(theirAddr);

    int receiverFd = accept(serverFd, (struct sockaddr *)&theirAddr, &size);
    if (receiverFd == -1) {
      continue;
    }

    char userRequest[4096];
    size_t userRequestLen = 0, prevRequestLen = 0;
    const char *method = NULL, *path = NULL;
    size_t method_len = 0, path_len = 0;
    int minor_version = 0;
    struct phr_header headers[100];
    size_t headerNum;
    int pret = -2;

    while (1) {
      ssize_t dataReceived = recv(receiverFd, userRequest + userRequestLen,
                                  sizeof(userRequest) - userRequestLen, 0);

      if (dataReceived <= 0) {
        if (dataReceived < 0)
          perror("recv");
        break;
      }

      prevRequestLen = userRequestLen;
      userRequestLen += (size_t)dataReceived;
      headerNum = sizeof(headers) / sizeof(headers[0]);

      pret = phr_parse_request(userRequest, userRequestLen, &method,
                               &method_len, &path, &path_len, &minor_version,
                               headers, &headerNum, prevRequestLen);

      if (pret > 0) {
        // Successfully parsed complete HTTP headers
        break;
      } else if (pret == -1) {
        fprintf(stderr, "Parse Error: malformed HTTP request\n");
        break;
      }

      // pret == -2: Request is incomplete, continue recv if space permits
      if (userRequestLen == sizeof(userRequest)) {
        fprintf(stderr, "Request header too large\n");
        break;
      }
    }

    if (pret > 0) {

      Request *userReq = newServerRequest(method, method_len, path, path_len,
                                          headers, headerNum);

      String tempStr = {.data = (char *)userReq->path.data,
                        .size = userReq->path.size};

      routeMatcher(userReq, routeNode, tempStr, receiverFd);

      destroyMap2(userReq->params);
      free(userReq);
    }

    close(receiverFd);
  }
  close(serverFd);
  printf("Server shut down\n");
}
