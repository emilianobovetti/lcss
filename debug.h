#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "tree.h"

#define CHECK_NULL(p) if ((p) == NULL) { fprintf(stderr, "NULL\n"); }

void print_label(tree_t *tree, node_t *node);

void print_node(tree_t *tree, node_t *node);

void print_tree(tree_t *tree);

#endif
