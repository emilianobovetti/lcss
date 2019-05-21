#include "tree.h"

node_t *new_node(void)
{
    substring_t *s = malloc(sizeof(substring_t));
    node_t *n = malloc(sizeof(node_t));

    s->left = 0;
    s->right = 0;

    n->edge_label = *s;

    n->parent =
    n->suffix_link =
    n->first_child =
    n->last_child =
    n->first_sibling = NULL;

    return n;
}

node_t *new_leaf(size_t left_label)
{
    substring_t *s = malloc(sizeof(substring_t));
    node_t *n = malloc(sizeof(node_t));

    s->left = left_label;
    // open transition
    s->right = -1;

    n->edge_label = *s;

    n->parent =
    n->suffix_link =
    n->first_child =
    n->first_sibling = NULL;

    return n;
}
