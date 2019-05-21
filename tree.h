#ifndef __TREE_H__
#define __TREE_H__

#include <stdlib.h>

typedef struct node
{
    int left_label, right_label;

    struct node *parent;
    struct node *suffix_link;

    struct node *first_child;
    struct node *last_child;
    struct node *next_sibling;
}
node_t;

typedef struct tree
{
    char *str;
    int str_length;

    struct node *root;
}
tree_t;

node_t *new_node(int left, int right);

node_t *new_leaf(int left);

#endif
