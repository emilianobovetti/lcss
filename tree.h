#ifndef __TREE_H__
#define __TREE_H__

#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct node
{
    int left_label, right_label;

    int idx; // TODO: just for debugging
    int depth;
    int num_leaves;
    int uniq_str_count;

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
    int last_idx;
    int num_strings;

    int cur_leaf_idx;
    int cur_node_idx; // TODO: just for debugging

    struct node ***leaves_matrix;

    struct node *aux;
    struct node *root;
}
tree_t;

node_t *new_node(int left, int right);

node_t *new_leaf(int left);

tree_t *new_tree(char *str, int num_strings);

void update_num_leaves(node_t *node);

int get_leaf_str_idx(tree_t *tree, node_t *node);

void post_process_tree(tree_t *tree);

void process_leaves_pair(tree_t *tree);

bool is_end_sym(char c);

node_t *lca(node_t *n1, node_t *n2);

#endif
