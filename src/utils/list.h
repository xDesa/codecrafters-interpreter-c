#ifndef CLOX_LIST_H
#define CLOX_LIST_H

#include "common.h"

typedef struct node_t {
  void* data;
  struct node_t* next;
} Node;

typedef struct list_t {
  Node* head;
  Node* tail;
  size_t size;
} List;

typedef void (*Iterator)(void* data);

static inline List new_list() {
  return (List) { NULL, NULL, 0 };
}

void list_append(List* list, void* data);

void list_foreach(List list, Iterator get_data);

void free_list(List* list, Iterator free_data);

#endif /* CLOX_LIST_H */