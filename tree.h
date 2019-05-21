#ifndef __TREE_H__
#define __TREE_H__

#include <stdlib.h>

#include "substring.h"

typedef struct node
{
    struct substring edge_label;

    struct node *parent;
    struct node *suffix_link;

    struct node *first_child;
    struct node *last_child;
    struct node *first_sibling;
}
node_t;

typedef struct tree
{
    char *str;
    size_t str_length;

    struct node *root;
}
tree_t;

/*
 * References a point inside a tree.
 * Since the edge `e` from node `A` to node `B`
 * is stored in `edge_label` inside node `B`,
 * this reference will always point to the child
 * node, and therefore it will never point to root.
 * That's why a `parent` reference is stored.
 */

typedef struct tree_ref
{
    struct node *node;
    struct node *parent;

    size_t length;
}
tree_ref_t;

#define NODE_IS_INTERNAL(n) (NULL != n->first_child)

node_t *new_node(void);

node_t *new_leaf(size_t left_label);

#endif
