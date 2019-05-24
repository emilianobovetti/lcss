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

typedef struct lcss_array_list
{
    struct node *current;

    struct lcss_array_list *next;
}
lcss_array_list_t;

node_t *new_node(int left, int right);

node_t *new_leaf(int left);

tree_t *new_tree(char *str, int num_strings);

int get_leaf_str_idx(tree_t *tree, node_t *node);

void post_process_tree(tree_t *tree);

void process_leaves_pair(tree_t *tree);

void compute_uniq_str_count(tree_t *tree);

bool is_end_sym(char c);

node_t *lca(node_t *n1, node_t *n2);

node_t *get_lcs(tree_t *tree);

lcss_array_list_t **get_lcss(tree_t *tree);

char *node_to_string(tree_t *tree, node_t *n);

#endif
