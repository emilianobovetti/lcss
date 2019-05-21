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

void find_tk_transition(char *str, node_t *start, int k, node_prev_sibling_t *res)
{
    node_t *child = start->first_child;
    node_t *prev = NULL;

    while (child != NULL)
    {
        if (str[child->left_label] == str[k])
        {
            res->node = child;
            res->prev_sibling = prev;

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
        find_tk_transition(str, s, k, &tk_search);
        // TODO: is sp always !NULL?
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
            tk_search.prev_sibling->next_sibling = r;

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
        find_tk_transition(str, s, p + 1, &tk_search);

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
        find_tk_transition(str, s, k, &tk_search);
        kp = tk_search.node->left_label;
        pp = tk_search.node->right_label;

        while (pp - kp <= p - k)
        {
            k = k + pp - kp + 1;
            s = tk_search.node;

            if (k <= p)
            {
                find_tk_transition(str, s, kp, &tk_search);
                kp = tk_search.node->left_label;
                pp = tk_search.node->right_label;
            }
        }

        res->node = s;
        res->left_ptr = k;
    }
}

void update(tree_t *tree, node_t *start, int k, int i, node_left_ptr_t *res)
{
    node_t *s = start;
    node_t *oldr = tree->root;

    node_endpoint_t endp = { .node = NULL, .is_endpoint = false };

    test_and_split(tree, s, k, i - 1, tree->str[i], &endp);

    while (!endp.is_endpoint)
    {
        node_t *r = endp.node;

        node_t *rp = new_leaf(i);
        rp->parent = r;
        r->last_child->next_sibling = rp;
        r->last_child = rp;

        if (oldr != tree->root)
        {
            oldr->suffix_link = r;
        }

        oldr = r;
        node_left_ptr_t suff = { .node = NULL, .left_ptr = 0 };
        canonize(tree, s->suffix_link, k, i - 1, &suff);
        s = suff.node;
        k = suff.left_ptr;

        test_and_split(tree, s, k, i - 1, tree->str[i], &endp);
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
     * `phase = 0 .. length(str)`
     */
    int phase = 0;

    node_t *root = new_node(0, 0);

    tree_t *tree = malloc(sizeof(tree_t));
    tree->str = str;
    tree->str_length = 1;
    tree->root = root;

    node_left_ptr_t s_k = { .node = root, .left_ptr = 1 };

    while (str[phase - 1] != '\0')
    {
        printf("phase: %d\n", phase);

        update(tree, s_k.node, s_k.left_ptr, phase, &s_k);
        canonize(tree, s_k.node, s_k.left_ptr, phase, &s_k);

        tree->str_length++;
        phase++;
    }

    return tree;
}

void main(void)
{
    ukkonen("hello, world");
}
