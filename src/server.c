#define _POSIX_C_SOURCE 202405L

#include "server.h"
#include "picohttpparser.h"
#include "vector.h"
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

Request *newServerRequest(const char *method, size_t method_len,
                          const char *path, size_t path_len,
                          struct phr_header *headers, size_t header_len) {

  Request *new = malloc(sizeof(Request));

  VECTOR_INIT(&(new->path));
  VECTOR_INIT(&(new->method));
  VECTOR_INIT(&(new->header));

  new->path.data = path;
  new->path.size = path_len;

  new->method.data = method;
  new->method.size = method_len;

  new->header.data = headers;
  new->header.size = header_len;

  return new;
}

void server(char *port) {
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

      printf("method is %s\n", userReq->method.data);
      printf("path is %s\n", userReq->path.data);
      printf("HTTP version is 1.%d\n", minor_version);
      printf("headers:\n");
      for (size_t i = 0; i < userReq->header.size; ++i) {

        struct phr_header *headers = userReq->header.data;
        printf("%.*s: %.*s\n", (int)headers[i].name_len, headers[i].name,
               (int)headers[i].value_len, headers[i].value);
      }

      // Optional:
      //   Send a minimal 200 OK response back
      const char resp[] =
          "HTTP/1.1 200 OK\r\nContent-Length: 2\r\nConnection: close\r\n\r\nOK";
      send(receiverFd, resp, sizeof(resp) - 1, 0);

      free(userReq);
    }

    close(receiverFd);
  }

  close(serverFd);
  printf("Server shut down\n");
}
