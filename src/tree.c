#include "tree.h"

node_t *new_node(int left, int right)
{
    node_t *n = malloc(sizeof(node_t));

    n->depth = 0;
    n->num_leaves = 0;
    n->lca_count = 0;
    n->uniq_str_count = 0;

    n->melting_point = 0;

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

    n->melting_point = 0;

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

/*
 * Checks if a char is considered an 'end symbol'.
 *
 * The end symbols are unique characters that are
 * appended at the end of every strings to delimitate
 * them.
 *
 * Those characters shouldn't appear inside the strings.
 */
bool is_end_sym(char c)
{
    return c < 31 || c > SCHAR_MAX;
}

/*
 * The last character of every leaf should be an
 * end symbol.
 *
 * Those symbols identify the strings, so we can
 * convert them to unique indexes.
 *
 * Every string has a string index if the
 * end symbols were set correctly.
 *
 * This index must go from 0 to num_strings - 1
 * in order to fill str_idx_to_leaf_list properly.
 */
int get_leaf_str_idx(tree_t *tree, node_t *node)
{
    return - tree->str[node->right_label] - 1;
}

/*
 * Given two nodes finds the LCA.
 *
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

/*
 * Given a node, sets node->num_leaves for that node
 * and recursively for all its children.
 */
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

/*
 * Given a node, frees its children.
 */
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

/*
 * C(v)           = S(v)       - U(v)
 * uniq_str_count = num_leaves - corr_fac
 *
 * corr_fac = n->lca_count + sum(lca_count of every child of n)
 *
 * Given node 'n':
 * - processes recursively the correction factors
 *      of the entire subtree rooted in 'n'
 * - sets the 'uniq_str_count' for every processed node
 * - returns the corr_fac of 'n'
 *
 * The 'uniq_str_count' is the number of the strings
 * that shares the current node.
 */
int process_corr_fac(node_t *node)
{
    node_t *first_child = node->first_child;

    if (first_child == NULL)
    {
        // lca_count is 0 for every leaf
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

#define MAX(a, b) (((a) > (b)) ? (a) : (b))

/*
 * Since 'uniq_str_count' represents the number of strings
 * that share a single node, it goes from 1 to tree->num_strings.
 *
 * A node with uniq_str_count = 1 represents a substring common to
 * a single string, while a node with uniq_str_count = num_strings
 * represents a substring common to all given strings.
 *
 * tree->str_count_to_max_depth hold an array that stores
 * the max depth for every uniq_str_count = 1 .. num_strings.
 */
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

/*
 * Sets 'uniq_str_count' for every node in tree.
 */
void compute_uniq_str_count(tree_t *tree)
{
    // C(v)           = S(v)       - U(v)
    // uniq_str_count = num_leaves - corr_fac
    process_corr_fac(tree->root);
}

/*
 * For every string we have a list of leafs
 * in tree->str_idx_to_leaf_list.
 *
 * We get the LCA of every pair of adjacent leafs
 * to set node->lca_count.
 */
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

#define LABEL_LENGTH(n) ((n)->right_label - (n)->left_label + 1)

/*
 * Given node 'n':
 * - sets n->depth
 * - trims unwanted end symbols in leafs (xyz$vwp# -> xyz$)
 * - adds every leaf to tree->str_idx_to_leaf_list
 * - processes the subtree rooted in 'n'
 * - processes the siblings of 'n'
 */
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

        /*
         * get_leaf_str_idx() returns, for every string,
         * a unique index that goes from 0 to num_strings.
         */
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

/*
 * Given tree 't':
 * - sets n->num_leaves (for every node n)
 * - sets n->depth (for every node n)
 * - trims unwanted end symbols in leafs (xyz$vwp# -> xyz$)
 * - adds every leaf to tree->str_idx_to_leaf_list
 * - sets n->lca_count (for every node n)
 * - sets n->uniq_str_count (for every node n)
 * - sets t->str_count_to_max_depth
 */
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
    process_leaves_pair(tree);
    compute_uniq_str_count(tree);

    tree->str_count_to_max_depth = calloc(tree->num_strings + 1, sizeof(int));

    for (int i = 0; i <= tree->num_strings; i++)
    {
        tree->str_count_to_max_depth[i] = 0;
    }

    fill_str_count_to_max_depth(tree, tree->root);
}

void fill_commons(tree_t *tree, int target_len, int range, node_list_t **arr_lst, node_t *node)
{
    if (node == NULL)
    {
        return;
    }

    int target_depth = target_len;

    if (target_len == INT_MAX)
    {
        // target_len = max_depth
        target_depth = tree->str_count_to_max_depth[node->uniq_str_count];
    }

    if (abs(target_depth - node->depth) <= range)
    {
        node_list_t *new_node = malloc(sizeof(node_list_t));

        new_node->current = node;
        new_node->next = arr_lst[node->uniq_str_count];

        arr_lst[node->uniq_str_count] = new_node;
    }

    fill_commons(tree, target_len, range, arr_lst, node->next_sibling);
    fill_commons(tree, target_len, range, arr_lst, node->first_child);
}

node_list_t **get_commons_by_length(tree_t *tree, int target_len, int range)
{
    node_list_t **arr_lst = calloc(tree->num_strings + 1, sizeof(node_list_t*));

    for (int i = 0; i <= tree->num_strings; i++)
    {
        arr_lst[i] = malloc(sizeof(node_list_t));
        arr_lst[i]->current = tree->root;
        arr_lst[i]->next = NULL;
    }

    fill_commons(tree, target_len, range, arr_lst, tree->root);

    return arr_lst;
}

void set_node_melting_point(tree_t *tree, node_t *node)
{
    if (node == NULL)
    {
        return;
    }

    node->melting_point = node->parent->melting_point;

    char *str = tree->str;

    for (int i = node->left_label; i <= node->right_label; i++)
    {
        if (is_end_sym(str[i]))
        {
            break;
        }

        switch (str[i])
        {
            case 'A': case 'a':
            case 'T': case 't':
                node->melting_point += 2;
                break;
            case 'G': case 'g':
            case 'C': case 'c':
                node->melting_point += 4;
                break;
            default:
                break;
        }
    }

    set_node_melting_point(tree, node->first_child);
    set_node_melting_point(tree, node->next_sibling);
}

void set_melting_points(tree_t *tree)
{
    node_t *cur_child = tree->root->first_child;

    while (cur_child != NULL)
    {
        set_node_melting_point(tree, cur_child);
        cur_child = cur_child->next_sibling;
    }
}

void fill_mps(tree_t *tree, int target_melt_pt, int range, node_list_t **arr_lst, node_t *node)
{
    if (node == NULL)
    {
        return;
    }

    if (abs(node->melting_point - target_melt_pt) <= range)
    {
        node_list_t *new_node = malloc(sizeof(node_list_t));

        new_node->current = node;
        new_node->next = arr_lst[node->uniq_str_count];

        arr_lst[node->uniq_str_count] = new_node;
    }

    fill_mps(tree, target_melt_pt, range, arr_lst, node->next_sibling);
    fill_mps(tree, target_melt_pt, range, arr_lst, node->first_child);
}

node_list_t **get_commons_by_melting_point(tree_t *tree, int target_melt_pt, int range)
{
    set_melting_points(tree);

    node_list_t **arr_lst = calloc(tree->num_strings + 1, sizeof(node_list_t*));

    for (int i = 0; i <= tree->num_strings; i++)
    {
        arr_lst[i] = malloc(sizeof(node_list_t));
        arr_lst[i]->current = tree->root;
        arr_lst[i]->next = NULL;
    }

    fill_mps(tree, target_melt_pt, range, arr_lst, tree->root);

    return arr_lst;
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
