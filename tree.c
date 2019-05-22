#include "tree.h"

#define LABEL_LENGTH(n) ((n)->right_label - (n)->left_label + 1)

node_t *new_node(int left, int right)
{
    node_t *n = malloc(sizeof(node_t));

    n->idx = INT_MIN;
    n->depth = -1;

    n->left_label = left;
    n->right_label = right;

    n->parent =
    n->suffix_link =
    n->first_child =
    n->last_child =
    n->next_sibling = NULL;

    return n;
}

node_t *new_leaf(tree_t *tree, int left)
{
    node_t *n = malloc(sizeof(node_t));

    n->idx = INT_MAX;
    n->depth = -1;

    n->left_label = left;
    // open transition
    n->right_label = INT_MAX;

    n->parent =
    n->suffix_link =
    n->first_child =
    n->next_sibling = NULL;

    tree->num_leaves++;

    return n;
}

tree_t *new_tree(char *str)
{
    tree_t *tree = malloc(sizeof(tree_t));
    node_t *aux = new_node(1, 0);
    node_t *root = new_node(-1, -1);

    tree->str = str;
    tree->last_idx = 0;

    tree->num_leaves = 0;
    tree->cur_leaf_idx = 0;
    tree->cur_node_idx = -1;

    tree->aux = aux;
    tree->root = root;
    aux->first_child = aux->last_child = root;
    root->parent = root->suffix_link = aux;
    root->first_child = root->last_child = NULL;

    return tree;
}

bool is_end_sym(char c)
{
    return c <= 31 || c >= SCHAR_MAX;
}

void post_process_node(tree_t *tree, node_t *node)
{
    if (node == NULL)
    {
        return;
    }

    char *str = tree->str;

    for (int i = node->left_label; i <= node->right_label; i++)
    {
        if (is_end_sym(str[i]))
        {
            node->right_label = i;
            break;
        }
    }

    node->depth = node->parent->depth + LABEL_LENGTH(node);

    if (node->first_child == NULL)
    {
        node->idx = tree->cur_leaf_idx++;
    }
    else
    {
        node->idx = tree->cur_node_idx--;
        post_process_node(tree, node->first_child);
    }

    post_process_node(tree, node->next_sibling);
}

void post_process_tree(tree_t *tree)
{
    tree->root->depth = 0;

    post_process_node(tree, tree->root);
}
