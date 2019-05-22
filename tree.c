#include "tree.h"

#define LABEL_LENGTH(n) ((n)->right_label - (n)->left_label + 1)

node_t *new_node(int left, int right)
{
    node_t *n = malloc(sizeof(node_t));

    n->idx = INT_MIN;
    n->depth = -1;
    n->num_leaves = 0;
    n->lca_count = 0;
    n->uniq_str_count = -1;

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

    n->idx = INT_MAX;
    n->depth = -1;
    n->num_leaves = -1;
    n->lca_count = -1;
    n->uniq_str_count = -1;

    n->left_label = left;
    // open transition
    n->right_label = INT_MAX;

    n->parent =
    n->suffix_link =
    n->first_child =
    n->next_sibling = NULL;

    return n;
}

tree_t *new_tree(char *str, int num_strings)
{
    tree_t *tree = malloc(sizeof(tree_t));
    node_t *aux = new_node(1, 0);
    node_t *root = new_node(-1, -1);

    tree->str = str;
    tree->last_idx = 0;
    tree->num_strings = num_strings;

    tree->cur_leaf_idx = 0;
    tree->cur_node_idx = -1;

    tree->aux = aux;
    tree->root = root;
    aux->first_child = aux->last_child = root;
    root->parent = root->suffix_link = aux;
    root->first_child = root->last_child = NULL;

    root->depth = 0;

    return tree;
}

void update_num_leaves(node_t *node)
{
    if (node == NULL)
    {
        return;
    }

    node->num_leaves++;

    update_num_leaves(node->parent);
}

bool is_end_sym(char c)
{
    return c <= 31 || c >= SCHAR_MAX;
}

int get_leaf_str_idx(tree_t *tree, node_t *node)
{
    return - tree->str[node->right_label] - 1;
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

        node_t** li = tree->leaves_matrix[get_leaf_str_idx(tree, node)];
        size_t cur_idx = (size_t) li[0];

        li[cur_idx++] = node;
        li[0] = (node_t*) cur_idx;
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
    tree->leaves_matrix = calloc(tree->num_strings, sizeof(node_t**));

    for (size_t i = 0; i < tree->num_strings; i++)
    {
        tree->leaves_matrix[i] = calloc(tree->root->num_leaves + 1, sizeof(node_t*));
        tree->leaves_matrix[i][0] = (node_t*) 1; // current index
    }

    post_process_node(tree, tree->root);
}

void process_leaves_pair(tree_t *tree)
{
    for (size_t i = 0; i < tree->num_strings; i++)
    {
        node_t** li = tree->leaves_matrix[i];
        size_t cur_idx = (size_t) li[0];

        for (size_t j = 1; j + 1 < cur_idx; j++)
        {
            node_t *n = lca(li[j], li[j + 1]);
            n->lca_count++;
        }
    }
}

void compute_uniq_str_count(tree_t *tree)
{
    // TODO
}

node_t *lca(node_t *n1, node_t *n2)
{
    if (n1 == n2)
    {
        return n1;
    }

    if (n1->depth > n2->depth)
    {
        return lca(n1->parent, n2);
    }
    else
    {
        return lca(n1, n2->parent);
    }
}
