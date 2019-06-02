#ifndef __TREE_H__
#define __TREE_H__

#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct node
{
    int left_label, right_label;

    int depth;
    int num_leaves;
    int lca_count;
    int uniq_str_count;

    int melting_point;

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

    /*
     * Each end symbol is in correspondence with a string index.
     * This data structure holds a list of every leafs that
     * contain the end symbol of a given string.
     */
    struct node ***str_idx_to_leaf_list;

    /*
     * Stores, for every uniq_str_count = 1 .. num_strings,
     * the max depth at wich a node with that uniq_str_count
     * is found.
     */
    int *str_count_to_max_depth;

    struct node *aux;
    struct node *root;
}
tree_t;

typedef struct node_list
{
    struct node *current;

    struct node_list *next;
}
node_list_t;

node_t *new_node(int left, int right);

node_t *new_leaf(int left);

tree_t *new_tree(char *str, int num_strings);

int get_leaf_str_idx(tree_t *tree, node_t *node);

void post_process_tree(tree_t *tree);

bool is_end_sym(char c);

node_list_t **get_commons_by_length(tree_t *tree, int target_len, int range);

node_list_t **get_commons_by_melting_point(tree_t *tree, int target_melt_pt, int range);

char *node_to_string(tree_t *tree, node_t *n);

#endif
