#include <limits.h>
#include <stdio.h>

#include "tree.h"


node_t *new_node(int left, int right)
{
    node_t *n = malloc(sizeof(node_t));

    n->left_label = left;
    n->right_label = right;

    n->parent =
    n->suffix_link =
    n->first_child =
    n->last_child =
    n->next_sibling = NULL;

    return n;
}

node_t *new_leaf(int left)
{
    node_t *n = malloc(sizeof(node_t));

    n->left_label = left;
    // open transition
    n->right_label = INT_MAX;

    n->parent =
    n->suffix_link =
    n->first_child =
    n->next_sibling = NULL;

    return n;
}

void print_label(tree_t *tree, node_t *node)
{
    if (node == tree->root)
    {
        printf("root");

        return;
    }

    int right_label = (node->right_label == INT_MAX) ? tree->last_idx : node->right_label;

    for (int i = node->left_label; i <= right_label; i++)
    {
        char c = tree->str[i];
        putchar(c == '\0' ? '$' : c);
    }

    printf(" [%d %d]", node->left_label, right_label);
}

void print_node(tree_t *tree, node_t *node)
{
    print_label(tree, node);

    node_t *sibling;

    for (sibling = node->next_sibling; sibling != NULL; sibling = sibling->next_sibling)
    {
        printf(" - ");
        print_label(tree, sibling);
    }

    if (node->first_child != NULL)
    {
        printf("\nchildren of ");
        print_label(tree, node);
        printf("\n");
        print_node(tree, node->first_child);
    }

    for (sibling = node->next_sibling; sibling != NULL; sibling = sibling->next_sibling)
    {
        printf("\nsiblings of ");
        print_label(tree, node);
        printf("\n");
        print_node(tree, sibling);
    }
}

void print_tree(tree_t *tree)
{
    print_node(tree, tree->root);
    printf("\n");
}
