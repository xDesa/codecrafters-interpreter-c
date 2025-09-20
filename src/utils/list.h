#ifndef CLOX_LIST_H
#define CLOX_LIST_H

#include "common.h"

typedef struct list_node_t {
  void* data;
  struct list_node_t* next;
} ListNode;

typedef struct list_t {
  ListNode* head;
  ListNode* tail;
  size_t size;
} List;

typedef void (*Iterator)(void* data);

static inline List new_list() {
  return (List) { NULL, NULL, 0 };
}

static inline void* list_get_last_data(List list) {
  return list.tail->data;
}

void list_append(List* list, void* data);

void list_foreach(List list, Iterator get_data);

void free_list(List* list, Iterator free_data);

#endif /* CLOX_LIST_H */