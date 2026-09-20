#ifndef LIST_H
#define LIST_H

#include <stdlib.h>

#define List(type)                                                             \
  struct {                                                                     \
    type *data;                                                                \
    size_t size;                                                               \
  }
#endif
