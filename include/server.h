#ifndef SERVER_H
#define SERVER_H

#include "picohttpparser.h"
#include "vector.h"
#include <stddef.h>

typedef struct serverRequest {
  Vector(const char) path;
  Vector(const char) method;
  Vector(struct phr_header) header;
} Request;

void server(char *);
Request *newServerRequest(const char *method, size_t method_len,
                          const char *path, size_t path_len,
                          struct phr_header *headers, size_t header_len);

#endif
