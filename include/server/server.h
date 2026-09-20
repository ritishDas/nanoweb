#ifndef SERVER_H
#define SERVER_H

#include "dataStructure/khash.h"
#include "dataStructure/list.h"
#include "picohttpparser.h"
#include "routeTrie.h"
#include "routeTypes.h"
#include <stddef.h>

typedef struct serverRequest {
  List(const char) path;
  List(const char) method;
  List(struct phr_header) header;
  khash_t(2) * params;
} Request;

void server(char *port, TrieNode *routeNode);

Request *newServerRequest(const char *method, size_t method_len,
                          const char *path, size_t path_len,
                          struct phr_header *headers, size_t header_len);

void newServerResponse(char *buffer, size_t *bufferSize, int status_code,
                       const char *content_type, const char *body);

#endif
