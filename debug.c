#include "debug.h"

void print_label_string(tree_t *tree, node_t *node)
{
    int left_label = node->left_label;
    int right_label = node->right_label;

    if (node->right_label >= INT_MAX)
    {
        right_label = tree->last_idx;
    }

    for (int i = left_label; i <= right_label; i++)
    {
        char c = tree->str[i];

        if (is_end_sym(c))
        {
            printf("$%d", -c);
        }
        else
        {
            putchar(c);
        }
    }

    putchar(' ');
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

    print_label_string(tree, node);

    if (node->right_label >= INT_MAX)
    {
        printf("[%d inf]", node->left_label);
    }
    else
    {
        printf("[%d %d]", node->left_label, node->right_label);
    }

    printf(" %d", node);
    //printf(" d(%d)", node->depth);
}

void print_node(tree_t *tree, node_t *node)
{
    if (node == NULL)
    {
        return;
    }

    printf("\n");
    print_label(tree, node->parent);
    printf(" -> ");
    print_label(tree, node);

    print_node(tree, node->next_sibling);
    print_node(tree, node->first_child);
}

void print_tree(tree_t *tree)
{
    printf("leaves: %d\n", tree->num_leaves);
    print_node(tree, tree->root);
    printf("\n");
}
