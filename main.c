#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>

#include "tree.h"
#include "ukkonen.h"

void main(void)
{
    char *strings[] = {
        "hello, world",
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

        cat[cat_idx] = end_sym;
        cat_idx++;
        end_sym--;
    }

    // don't forget to bring a towel
    cat[cat_idx] = '\0';

    tree_t *tree = build_tree(cat, n_str);
    post_process_tree(tree);
    process_leaves_pair(tree);
    compute_uniq_str_count(tree);

    node_t **lcss = get_lcss(tree);

    for (int i = 1; i <= tree->num_strings; i++)
    {
        char *i_lcs = to_string(tree, lcss[i]);
        printf("%d -> %s\n", i, i_lcs);
        free(i_lcs);
    }
}
