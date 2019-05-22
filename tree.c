#include <limits.h>
#include <stdio.h>
#include <stdbool.h>

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

tree_t *new_tree(void)
{
    tree_t *tree = malloc(sizeof(tree_t));
    node_t *aux = new_node(1, 0);
    node_t *root = new_node(-1, -1);

    tree->aux = aux;
    tree->root = root;
    tree->cur_string = NULL;
    aux->first_child = aux->last_child = root;
    root->parent = root->suffix_link = aux;
    root->first_child = root->last_child = NULL;

    return tree;
}

int get_right_label(tree_t *tree, node_t *node)
{
    int right_label = node->right_label;

    return right_label == INT_MAX ? tree->last_idx : right_label;
}

bool is_empty_leaf(tree_t *tree, node_t *node)
{
    int right_label = get_right_label(tree, node);

    return node != tree->root && node->left_label == right_label && tree->cur_string[right_label] == '\0';
}

void print_label(tree_t *tree, node_t *node)
{
    if (node == tree->aux)
    {
        printf("aux");
        return;
    }

    if (node == tree->root)
    {
        printf("root");
        return;
    }

    int right_label = get_right_label(tree, node);

    for (int i = node->left_label; i <= right_label; i++)
    {
        putchar(tree->cur_string[i] == '\0' ? '$' : tree->cur_string[i]);
    }

    printf(" [%d %d]", node->left_label + 1, right_label + 1);
}

void print_node(tree_t *tree, node_t *node)
{
    printf("\n");
    print_label(tree, node->parent);
    printf(" -> ");
    print_label(tree, node);

    if (node->next_sibling != NULL)
    {
        print_node(tree, node->next_sibling);
    }

    if (node->first_child != NULL)
    {
        print_node(tree, node->first_child);
    }
}

void print_tree(tree_t *tree)
{
    print_node(tree, tree->root);
    printf("\n");
}
