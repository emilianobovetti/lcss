#include "ukkonen.h"

typedef struct node__left_ptr
{
    node_t *node;

    int left_ptr;
}
node__left_ptr_t;

typedef struct node__is_endpoint
{
    node_t *node;

    bool is_endpoint;
}
node__is_endpoint_t;

typedef struct node__prev_sibling
{
    node_t *node;

    node_t *prev_sibling;
}
node__prev_sibling_t;

void find_transition_and_prev(tree_t *tree, char t, node__prev_sibling_t *data)
{
    node_t *node = data->node;
    data->prev_sibling = NULL;

    if (node == tree->aux)
    {
        data->node = tree->root;

        return;
    }

    node_t *prev = NULL;
    node_t *child = node->first_child;

    while (child != NULL)
    {
        if (tree->str[child->left_label] == t)
        {
            data->prev_sibling = prev;
            data->node = child;

            return;
        }

        prev = child;
        child = child->next_sibling;
    }

    data->node = NULL;
}

node_t *find_transition(tree_t *tree, node_t *node, char t)
{
    node__prev_sibling_t t_search = { .node = node, .prev_sibling = NULL };
    find_transition_and_prev(tree, t, &t_search);

    return t_search.node;
}

void test_and_split(tree_t *tree, int k, int p, char t, node__is_endpoint_t *data)
{
    char *str = tree->str;
    node_t *s = data->node;

    data->is_endpoint = false;

    if (k <= p)
    {
        node__prev_sibling_t t_search = { .node = s, .prev_sibling = NULL };
        find_transition_and_prev(tree, str[k], &t_search);
        node_t *sp = t_search.node;
        int kp = sp->left_label;

        if (t == str[kp + p - k + 1])
        {
            data->is_endpoint = true;
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

            if (s->first_child == sp)
            {
                s->first_child = r;
            }

            if (s->last_child == sp)
            {
                s->last_child = r;
            }

            data->node = r;
        }
    }
    else
    {
        if (find_transition(tree, s, t) != NULL)
        {
            data->is_endpoint = true;
        }
    }
}

void canonize(tree_t *tree, int p, node__left_ptr_t *data)
{
    int k = data->left_ptr;
    node_t *s = data->node;
    char *str = tree->str;
    int kp, pp;

    if (p < k)
    {
        data->node = s;
        data->left_ptr = k;
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

        data->node = s;
        data->left_ptr = k;
    }
}

void update(tree_t *tree, int i, node__left_ptr_t *data)
{
    int k = data->left_ptr;
    node_t *s = data->node;
    node_t *oldr = tree->root;
    char ti = tree->str[i];

    node__is_endpoint_t split = { .node = s, .is_endpoint = false };

    test_and_split(tree, k, i - 1, ti, &split);

    while (!split.is_endpoint)
    {
        node_t *r = split.node;

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
        node__left_ptr_t suff = { .node = s->suffix_link, .left_ptr = k };
        canonize(tree, i - 1, &suff);
        s = suff.node;
        k = suff.left_ptr;

        split.node = suff.node;

        test_and_split(tree, k, i - 1, ti, &split);
    }

    if (oldr != tree->root)
    {
        oldr->suffix_link = s;
    }

    data->node = s;
    data->left_ptr = k;
}

tree_t *build_tree(char *str, int num_strings)
{
    tree_t *tree = new_tree(str, num_strings);

    int phase = 0;

    node__left_ptr_t s_k = { .node = tree->root, .left_ptr = 0 };

    while (str[phase] != '\0')
    {
        update(tree, phase, &s_k);
        canonize(tree, phase, &s_k);

        phase++;
    }

    tree->last_idx = phase - 1;

    return tree;
}
