#include "tree.h"

#define LABEL_LENGTH(n) ((n)->right_label - (n)->left_label + 1)
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

node_t *new_node(int left, int right)
{
    node_t *n = malloc(sizeof(node_t));

    n->depth = 0;
    n->num_leaves = 0;
    n->lca_count = 0;
    n->uniq_str_count = 0;

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

    n->depth = 0;
    n->num_leaves = 0;
    n->lca_count = 0;
    n->uniq_str_count = 0;

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

    tree->aux = aux;
    tree->root = root;
    aux->first_child = aux->last_child = root;
    root->parent = root->suffix_link = aux;
    root->first_child = root->last_child = NULL;

    root->depth = 0;

    return tree;
}

bool is_end_sym(char c)
{
    return c < 31 || c > SCHAR_MAX;
}

int get_leaf_str_idx(tree_t *tree, node_t *node)
{
    return - tree->str[node->right_label] - 1;
}

int process_num_leaves(node_t *node)
{
    node_t *first_child = node->first_child;

    if (first_child == NULL)
    {
        return 1;
    }

    node->num_leaves = process_num_leaves(first_child);
    node_t *next_child = first_child->next_sibling;

    while (next_child != NULL)
    {
        node->num_leaves += process_num_leaves(next_child);
        next_child = next_child->next_sibling;
    }

    return node->num_leaves;
}

void free_children(node_t *node)
{
    node_t *cur = node->first_child;
    node_t *tmp;

    while (cur != NULL)
    {
        tmp = cur->next_sibling;
        free_children(cur);
        free(cur);

        cur = tmp;
    }

    node->first_child = NULL;
}

void post_process_node(tree_t *tree, node_t *node)
{
    node->depth = node->parent->depth + LABEL_LENGTH(node);

    if (node->first_child == NULL)
    {
        char *str = tree->str;

        /*
         * Leafs with more than one end symbol need to be trimmed:
         *  xyz$vwp# -> xyz$
         *
         * The condition `i <= node->right_label` isn't necessary
         * since the right_label on leafs is set to an arbitrary
         * high number by default.
         */
        for (int i = node->left_label; true; i++)
        {
            if (is_end_sym(str[i]))
            {
                node->right_label = i;
                break;
            }
        }

        node_t** leafs = tree->str_idx_to_leaf_list[get_leaf_str_idx(tree, node)];
        size_t cur_idx = (size_t) leafs[0];

        leafs[cur_idx++] = node;
        leafs[0] = (node_t*) cur_idx;
    }
    else
    {
        post_process_node(tree, node->first_child);
    }

    if (node->next_sibling != NULL)
    {
        post_process_node(tree, node->next_sibling);
    }
}

void post_process_tree(tree_t *tree)
{
    process_num_leaves(tree->root);

    tree->str_idx_to_leaf_list = calloc(tree->num_strings, sizeof(node_t**));

    for (size_t i = 0; i < tree->num_strings; i++)
    {
        tree->str_idx_to_leaf_list[i] = calloc(tree->root->num_leaves + 1, sizeof(node_t*));
        tree->str_idx_to_leaf_list[i][0] = (node_t*) 1; // current index
    }

    post_process_node(tree, tree->root);
}

void process_leaves_pair(tree_t *tree)
{
    for (size_t i = 0; i < tree->num_strings; i++)
    {
        node_t** leafs = tree->str_idx_to_leaf_list[i];
        size_t cur_idx = (size_t) leafs[0];

        for (size_t j = 1; j + 1 < cur_idx; j++)
        {
            node_t *n = lca(leafs[j]->parent, leafs[j + 1]->parent);
            n->lca_count++;
        }
    }
}

int process_corr_fac(node_t *node)
{
    node_t *first_child = node->first_child;

    if (first_child == NULL)
    {
        return 0;
    }

    int corr_fac = process_corr_fac(first_child);
    node_t *next_child = first_child->next_sibling;

    while (next_child != NULL)
    {
        corr_fac += process_corr_fac(next_child);
        next_child = next_child->next_sibling;
    }

    corr_fac += node->lca_count;
    node->uniq_str_count = node->num_leaves - corr_fac;

    return corr_fac;
}

void fill_str_count_to_max_depth(tree_t *tree, node_t *node)
{
    if (node == NULL)
    {
        return;
    }

    int sc = node->uniq_str_count;
    int cur = tree->str_count_to_max_depth[sc];

    tree->str_count_to_max_depth[sc] = MAX(cur, node->depth);

    fill_str_count_to_max_depth(tree, node->next_sibling);
    fill_str_count_to_max_depth(tree, node->first_child);
}

void compute_uniq_str_count(tree_t *tree)
{
    // C(v)           = S(v)       - U(v)
    // uniq_str_count = num_leaves - corr_fac
    process_corr_fac(tree->root);

    tree->str_count_to_max_depth = calloc(tree->num_strings + 1, sizeof(int));

    for (int i = 0; i <= tree->num_strings; i++)
    {
        tree->str_count_to_max_depth[i] = 0;
    }

    fill_str_count_to_max_depth(tree, tree->root);
}

/*
 * TODO: efficient implementation
 */
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

node_t *get_max(node_t *n1, node_t *n2)
{
    if (n1->uniq_str_count > n2->uniq_str_count)
    {
        return n1;
    }

    if (n2->uniq_str_count > n1->uniq_str_count)
    {
        return n2;
    }

    if (n1->depth > n2->depth)
    {
        return n1;
    }

    return n2;
}

node_t *node_lcs(node_t *node)
{
    node_t *max = node;

    if (node->next_sibling != NULL)
    {
        max = get_max(node, node_lcs(node->next_sibling));
    }

    if (node->first_child != NULL)
    {
        max = get_max(node, node_lcs(node->first_child));
    }

    return max;
}

node_t *get_lcs(tree_t *tree)
{
    return node_lcs(tree->root);
}

void fill_lcss(tree_t *tree, node_t *node, lcss_array_list_t **lcss)
{
    if (node == NULL)
    {
        return;
    }

    int max_depth = tree->str_count_to_max_depth[node->uniq_str_count];

    if (node->depth == max_depth)
    {
        lcss_array_list_t *new_lcs = malloc(sizeof(lcss_array_list_t));

        new_lcs->current = node;
        new_lcs->next = lcss[node->uniq_str_count];

        lcss[node->uniq_str_count] = new_lcs;
    }

    fill_lcss(tree, node->next_sibling, lcss);
    fill_lcss(tree, node->first_child, lcss);
}

lcss_array_list_t **get_lcss(tree_t *tree)
{
    lcss_array_list_t **lcss = calloc(tree->num_strings + 1, sizeof(lcss_array_list_t *));

    for (int i = 0; i <= tree->num_strings; i++)
    {
        lcss[i] = malloc(sizeof(lcss_array_list_t));
        lcss[i]->current = tree->root;
        lcss[i]->next = NULL;
    }

    fill_lcss(tree, tree->root, lcss);

    return lcss;
}

int label_cpy(node_t *node, char *str, char* out, int idx)
{
    int left_label = node->left_label;
    int right_label = node->right_label;

    if (is_end_sym(str[right_label]))
    {
        if (right_label - left_label == 0)
        {
            return idx;
        }

        right_label--;
    }

    for (int i = right_label; i >= left_label; i--)
    {
        out[idx--] = str[i];
    }

    return idx;
}

char *node_to_string(tree_t *tree, node_t *node)
{
    char *out = calloc(node->depth, 1); // sizeof(char)
    int idx = node->depth - 1;

    out[idx--] = '\0';

    if (idx < 0)
    {
        return out;
    }

    node_t *cur = node;

    while (cur != tree->aux)
    {
        idx = label_cpy(cur, tree->str, out, idx);

        cur = cur->parent;
    }

    return out;
}
