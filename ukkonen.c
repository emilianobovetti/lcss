#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "tree.h"
#include "substring.h"

tree_t *ukkonen(char *str)
{
    /*
     * `phase = 0 .. length(str)`
     *
     * Phase 0 is executed implicitly during setup.
     */
    size_t phase = 1;

    /*
     * `extension = 0 .. length(str) - 1`
     *
     * Phase 0 is executed implicitly during setup.
     */
    size_t extension = 0;
    size_t leaf_edges_count = 1;

    node_t *root = new_node();
    node_t *first_leaf = new_leaf(0);

    first_leaf->parent = root;
    root->first_child = first_leaf;
    root->last_child = first_leaf;

    tree_t *tree = malloc(sizeof(tree_t));
    tree->str = str;
    tree->str_length = 0;
    tree->root = root;

    tree_ref_t active_point = {
        .node = first_leaf,
        .parent = root,
        .length = 0
    };

    while (str[phase-1] != '\0')
    {
        char cur_chr = str[phase];

        substring_t cur_sub = {
            .left = 0, .right = phase
        };

        /*
         * We skip the first `leaf_edges_count` extensions,
         * where rule #1 applies.
         */
        extension = leaf_edges_count - 1;
        tree->str_length++;

        while (extension < phase)
        {
            substring_t cur_suf = {
                .left = extension, .right = phase
            };

            /*
             * Encountered an internal node while walking down,
             * updating `active_point`.
             */

            /*
             * If rule #3 applies, the current phase ends,
             * and active point's length gets incremented.
             */

            extension++;
        }

        phase++;
    }

    return tree;
}

typedef struct node_left_ptr
{
    node_t *node;
    size_t left_ptr;
}
node_left_ptr_t;

node_left_ptr_t update(node_t* root, node_t *act_n, substring_t ext_sub)
{
    substring_t cur_sub = { .left = ext_sub.left, .right = ext_sub.right - 1 };

    node_t *oldr = root;
}

void main(void)
{
    printf("hello, world\n");
}
