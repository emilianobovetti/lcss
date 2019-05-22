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
    char *cur_string;
    int last_idx;

    struct node *aux;
    struct node *root;
}
tree_t;

node_t *new_node(int left, int right);

node_t *new_leaf(int left);

tree_t *new_tree(void);

void print_tree(tree_t *tree);

#endif
