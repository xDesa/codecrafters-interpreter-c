#ifndef CLOX_BST
#define CLOX_BST

#include "common.h"

typedef struct bst_node_t {
  void* data;
  struct bst_node_t* left;
  struct bst_node_t* right;
} BstNode;

typedef int (*BstCmp)(void* left, void* right);

typedef void (*BstDataFree)(void* data);

typedef struct {
  BstNode* root;
  BstCmp cmp_fn;
  BstDataFree free_data_fn;
} BinarySearchTree;

static inline BinarySearchTree new_binary_search_tree(BstCmp cmp_fn, BstDataFree free_data_fn) {
  return (BinarySearchTree) { NULL, cmp_fn, free_data_fn };
}

void* bst_search(BinarySearchTree tree, void* data);

void bst_insert(BinarySearchTree* tree, void* data);

void free_bst(BinarySearchTree* tree);

#endif /* CLOX_BST */