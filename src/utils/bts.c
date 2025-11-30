#include <stdlib.h>
#include "bst.h"
#include "common.h"
#include "mem.h"
#include "panic.h"

void* bst_search(BinarySearchTree tree, void* data) {
  BstNode* curr = tree.root;

  while (curr != NULL) {
    int cmp_res = tree.cmp_fn(curr->data, data);

    if (cmp_res == 0) {
      break;
    } else if (cmp_res > 0) {
      curr = curr->right;
    } else {
      curr = curr->left;
    }
  }

  if (curr == NULL) {
    return NULL;
  }

  return curr->data;
}

void bst_insert(BinarySearchTree* tree, void* data) {
  if (tree == NULL) {
    panic("null binary search tree pointer");
  }

  BstNode* parent = NULL;
  BstNode* curr = tree->root;

  while (curr != NULL) {
    parent = curr;

    if (tree->cmp_fn(curr->data, data) > 0) {
      curr = curr->right;
    } else {
      curr = curr->left;
    }
  }

  BstNode* new_node = xmalloc(sizeof(BstNode));
  new_node->data = data;
  new_node->left = NULL;
  new_node->right = NULL;

  if (parent == NULL) {
    tree->root = new_node;
  } else if (tree->cmp_fn(parent->data, data) > 0) {
    parent->right = new_node;
  } else {
    parent->left = new_node;
  }
}

static void recursive_node_free(BstNode* root, BstDataFree free_data) {
  if (root == NULL) {
    return;
  }

  recursive_node_free(root->left, free_data);
  recursive_node_free(root->right, free_data);

  if (free_data != NULL) {
    free_data(root->data);
  }

  free(root);
}

void free_bst(BinarySearchTree* tree) {
  recursive_node_free(tree->root, tree->free_data_fn);
  tree->root = NULL;
}
