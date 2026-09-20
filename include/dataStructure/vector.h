#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>

#define Vector(type)                                                           \
  struct {                                                                     \
    type *data;                                                                \
    size_t size;                                                               \
    size_t capacity;                                                           \
  }

#define VECTOR_INIT(v)                                                         \
  do {                                                                         \
    (v)->data = NULL;                                                          \
    (v)->size = 0;                                                             \
    (v)->capacity = 0;                                                         \
  } while (0)

#define VECTOR_PUSH(v, val)                                                    \
  do {                                                                         \
    if ((v)->size >= (v)->capacity) {                                          \
      size_t _new_cap = (v)->capacity ? (v)->capacity * 2 : 4;                 \
      void *_ptr = realloc((v)->data, _new_cap * sizeof(*(v)->data));          \
      if (_ptr) {                                                              \
        (v)->data = _ptr;                                                      \
        (v)->capacity = _new_cap;                                              \
        (v)->data[(v)->size++] = (val);                                        \
      } else {                                                                 \
        perror("Vector reallocation failed");                                  \
        exit(1);                                                               \
      }                                                                        \
    } else {                                                                   \
      (v)->data[(v)->size++] = (val);                                          \
    }                                                                          \
  } while (0)

#define VECTOR_POP(v)                                                          \
  do {                                                                         \
    if ((v)->size > 0) {                                                       \
      (v)->size--;                                                             \
    }                                                                          \
  } while (0)

#define VECTOR_FREE(v)                                                         \
  do {                                                                         \
    free((v)->data);                                                           \
    (v)->data = NULL;                                                          \
    (v)->size = 0;                                                             \
    (v)->capacity = 0;                                                         \
  } while (0)

#endif
