#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>

#include "tree.h"
#include "debug.h"

typedef struct node_left_ptr
{
    node_t *node;

    int left_ptr;
}
node_left_ptr_t;

typedef struct node_endpoint
{
    node_t *node;

    bool is_endpoint;
}
node_endpoint_t;

typedef struct node_prev_sibling
{
    node_t *node;

    node_t *prev_sibling;
}
node_prev_sibling_t;

void find_transition_and_prev(tree_t *tree, node_t *node, char t, node_prev_sibling_t *res)
{
    if (node == tree->aux)
    {
        res->prev_sibling = NULL;
        res->node = tree->root;

        return;
    }

    node_t *prev = NULL;
    node_t *child = node->first_child;

    while (child != NULL)
    {
        if (tree->str[child->left_label] == t)
        {
            res->prev_sibling = prev;
            res->node = child;

            return;
        }

        prev = child;
        child = child->next_sibling;
    }
}

node_t *find_transition(tree_t *tree, node_t *node, char t)
{
    node_prev_sibling_t t_search = { .node = NULL, .prev_sibling = NULL };
    find_transition_and_prev(tree, node, t, &t_search);

    return t_search.node;
}

void test_and_split(tree_t *tree, node_t *node, int k, int p, char t, node_endpoint_t *res)
{
    char *str = tree->str;
    node_t *s = node;

    res->node = s;
    res->is_endpoint = false;

    if (k <= p)
    {
        node_prev_sibling_t t_search = { .node = NULL, .prev_sibling = NULL };
        find_transition_and_prev(tree, s, str[k], &t_search);
        node_t *sp = t_search.node;
        int kp = sp->left_label;

        if (t == str[kp + p - k + 1])
        {
            res->is_endpoint = true;
        }
        else
        {
            node_t *r = new_node(kp, kp + p - k);

            r->parent = s;
            r->first_child = r->last_child = sp;
            r->next_sibling = sp->next_sibling;

            if (t_search.prev_sibling != NULL)
            {
                t_search.prev_sibling->next_sibling = r;
            }

            sp->left_label = kp + p - k + 1;
            sp->parent = r;
            sp->next_sibling = NULL;

            CHECK_NULL(s->first_child);

            if (s->first_child == sp)
            {
                s->first_child = r;
            }

            if (s->last_child == sp)
            {
                s->last_child = r;
            }

            res->node = r;
        }
    }
    else
    {
        if (find_transition(tree, s, t) != NULL)
        {
            res->is_endpoint = true;
        }
    }
}

void canonize(tree_t *tree, node_t *node, int k, int p, node_left_ptr_t *res)
{
    node_t *s = node;
    char *str = tree->str;
    int kp, pp;

    if (p < k)
    {
        res->node = s;
        res->left_ptr = k;
    }
    else
    {
        node_t *tk_transition = find_transition(tree, s, str[k]);
        kp = tk_transition->left_label;
        pp = tk_transition->right_label;

        while (pp - kp <= p - k)
        {
            k = k + pp - kp + 1;
            s = tk_transition;

            if (k <= p)
            {
                tk_transition = find_transition(tree, s, str[k]);
                kp = tk_transition->left_label;
                pp = tk_transition->right_label;
            }
        }

        res->node = s;
        res->left_ptr = k;
    }
}

void update(tree_t *tree, node_t *node, int k, int i, node_left_ptr_t *res)
{
    node_t *s = node;
    node_t *oldr = tree->root;
    char ti = tree->str[i];

    node_endpoint_t split = { .node = NULL, .is_endpoint = false };

    test_and_split(tree, s, k, i - 1, ti, &split);

    while (!split.is_endpoint)
    {
        node_t *r = split.node;

        node_t *rp = new_leaf(i);
        rp->parent = r;
        update_num_leaves(r);

        if (r->last_child == NULL)
        {
            r->first_child = r->last_child = rp;
        }
        else
        {
            r->last_child->next_sibling = rp;
            r->last_child = rp;
        }

        if (oldr != tree->root)
        {
            oldr->suffix_link = r;
        }

        oldr = r;
        node_left_ptr_t suff = { .node = NULL, .left_ptr = 0 };
        canonize(tree, s->suffix_link, k, i - 1, &suff);
        s = suff.node;
        k = suff.left_ptr;

        test_and_split(tree, s, k, i - 1, ti, &split);
    }

    if (oldr != tree->root)
    {
        oldr->suffix_link = s;
    }

    res->node = s;
    res->left_ptr = k;
}

tree_t *build_tree(char *str, int num_strings)
{
    tree_t *tree = new_tree(str, num_strings);

    node_left_ptr_t s_k = { .node = tree->root, .left_ptr = 0 };

    int phase = 0;

    while (str[phase] != '\0')
    {
        update(tree, s_k.node, s_k.left_ptr, phase, &s_k);
        canonize(tree, s_k.node, s_k.left_ptr, phase, &s_k);

        phase++;
    }

    tree->last_idx = phase - 1;

    post_process_tree(tree);
    process_leaves_pair(tree);

    return tree;
}

void main(void)
{
    char *strings[] = {
        "xxxxx",
        "xxxxx",
        NULL
    };

    int n_str;

    // 1 for \0
    size_t total_len = 1;

    for (n_str = 0; strings[n_str] != NULL; n_str++)
    {
        // +1 for str unique end symbol
        total_len += strlen(strings[n_str]) + 1;
    }

    char *cat = calloc(total_len, sizeof(char));
    size_t cat_idx = 0;
    char end_sym = UCHAR_MAX;

    for (n_str = 0; strings[n_str] != NULL; n_str++)
    {
        if (!is_end_sym(end_sym))
        {
            fprintf(stderr, "Too many strings\n");
            fprintf(stderr, "I can't handle more than %d strings :(\n", UCHAR_MAX - SCHAR_MAX);
            fprintf(stderr, "(char '%d' isn't a valid end symbol)\n", end_sym);
            return;
        }

        char *cur_str = strings[n_str];
        size_t j = 0;

        while (cur_str[j] != '\0')
        {
            cat[cat_idx] = cur_str[j];

            cat_idx++;
            j++;
        }

        cat[cat_idx] = end_sym--;
        cat_idx++;
    }

    // don't forget to bring a towel
    cat[cat_idx] = '\0';

    tree_t *tree = build_tree(cat, n_str);
    print_tree(tree);
}
