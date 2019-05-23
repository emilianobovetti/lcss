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
            printf("$%d", get_leaf_str_idx(tree, node));
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

    if (node->first_child != NULL)
    {
        printf("C=%d ", node->uniq_str_count);
    }

    printf("%d ", node->idx);

    if (node->right_label >= INT_MAX)
    {
        printf("[%d inf] ", node->left_label);
    }
    else
    {
        printf("[%d %d] ", node->left_label, node->right_label);
    }

    print_label_string(tree, node);

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

void print_leaves_matrix(tree_t *tree)
{
    printf("leaves matrix\n");

    for (size_t i = 0; i < tree->num_strings; i++)
    {
        node_t** li = tree->leaves_matrix[i];
        size_t cur_idx = (size_t) li[0];

        printf("str %d:", i);

        for (size_t j = 1; j < cur_idx; j++)
        {
            printf(" %2d", li[j]->idx);
        }

        printf("\n");
    }
}

void print_tree(tree_t *tree)
{
    //print_leaves_matrix(tree);

    print_node(tree, tree->root);
    printf("\n");
}
