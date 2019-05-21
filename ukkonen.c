#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "tree.h"

#define CHECK_NULL(p) if ((p) == NULL) { printf("NULL\n"); }

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

void find_transition(tree_t *tree, node_t *node, char t, node_prev_sibling_t *res)
{
    if (node == tree->aux)
    {
        res->prev_sibling = NULL;
        res->node = tree->root;

        return;
    }

    char *str = tree->str;

    node_t *prev = NULL;
    node_t *child = node->first_child;

    while (child != NULL)
    {
        if (str[child->left_label] == t)
        {
            res->prev_sibling = prev;
            res->node = child;

            return;
        }

        prev = child;
        child = child->next_sibling;
    }
}

void test_and_split(tree_t *tree, node_t *node, int k, int p, char t, node_endpoint_t *res)
{
    char *str = tree->str;
    node_t *s = node;

    res->node = s;
    res->is_endpoint = false;

    if (k <= p)
    {
        node_prev_sibling_t tk_search = { .node = NULL, .prev_sibling = NULL };
        find_transition(tree, s, str[k], &tk_search);
        node_t *sp = tk_search.node;
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
            if (tk_search.prev_sibling != NULL)
            {
                tk_search.prev_sibling->next_sibling = r;
            }

            sp->left_label = kp + p - k + 1;
            sp->parent = r;
            sp->first_child = sp->last_child = NULL;
            sp->next_sibling = NULL;

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
        node_prev_sibling_t tk_search = { .node = NULL, .prev_sibling = NULL };
        find_transition(tree, s, t, &tk_search);

        if (tk_search.node != NULL)
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
        node_prev_sibling_t tk_search = { .node = NULL, .prev_sibling = NULL };
        find_transition(tree, s, str[k], &tk_search);
        kp = tk_search.node->left_label;
        pp = tk_search.node->right_label;

        while (pp - kp <= p - k)
        {
            k = k + pp - kp + 1;
            s = tk_search.node;

            if (k <= p)
            {
                find_transition(tree, s, str[k], &tk_search);
                kp = tk_search.node->left_label;
                pp = tk_search.node->right_label;
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

    node_endpoint_t endp = { .node = NULL, .is_endpoint = false };

    test_and_split(tree, s, k, i - 1, ti, &endp);

    while (!endp.is_endpoint)
    {
        node_t *r = endp.node;

        node_t *rp = new_leaf(i);
        rp->parent = r;

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

        test_and_split(tree, s, k, i - 1, ti, &endp);
    }

    if (oldr != tree->root)
    {
        oldr->suffix_link = s;
    }

    res->node = s;
    res->left_ptr = k;
}

tree_t *ukkonen(char *str)
{
    /*
     * `phase = 1 .. length(str)`
     */
    int phase = 1;

    tree_t *tree = malloc(sizeof(tree_t));
    node_t *aux = new_node(1, 0);
    node_t *root = new_node(-1, -1);
    node_t *fst_leaf = new_leaf(0);

    tree->str = str;
    tree->aux = aux;
    tree->root = root;
    aux->first_child = aux->last_child = root;
    root->parent = root->suffix_link = aux;

    root->first_child = root->last_child = fst_leaf;
    fst_leaf->parent = root;

    node_left_ptr_t s_k = { .node = root, .left_ptr = 1 };

    while (str[phase - 1] != '\0')
    {
        update(tree, s_k.node, s_k.left_ptr, phase, &s_k);
        canonize(tree, s_k.node, s_k.left_ptr, phase, &s_k);

        phase++;
    }

    tree->last_idx = phase - 1;

    return tree;
}

void main(void)
{
    tree_t *tree = ukkonen("abcabx");
    print_tree(tree);
}
