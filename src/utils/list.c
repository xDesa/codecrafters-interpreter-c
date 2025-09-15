#include "list.h"
#include <stdlib.h>
#include "mem.h"

void list_append(List* list, void* data) {
  if (list == NULL) {
    return;
  }

  Node* new_node = xmalloc(sizeof(Node));
  new_node->data = data;
  new_node->next = NULL;

  if (list->head == NULL) {
    list->head = new_node;
  }
  if (list->tail != NULL) {
    list->tail->next = new_node;
  }

  list->tail = new_node;
  list->size++;
}

void list_foreach(List list, Iterator get_data) {
  if (list.head == NULL || get_data == NULL) {
    return;
  }

  for (Node* curr = list.head; curr != NULL; curr = curr->next) {
    get_data(curr->data);
  }
}

void free_list(List* list, Iterator free_data) {
  if (list == NULL || list->head == NULL) {
    return;
  }

  Node* curr = list->head;

  while (curr != NULL) {
    Node* tmp = curr;

    if (free_data != NULL) {
      free_data(curr->data);
    }
    curr = curr->next;

    free(tmp);
  }

  list->head = NULL;
  list->tail = NULL;
  list->size = 0;
}
