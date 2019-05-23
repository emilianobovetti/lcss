#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#include "tree.h"
#include "ukkonen.h"

typedef struct joined_str
{
    char *ptr;

    size_t num_strings;
}
joined_str_t;

node_t **tree_to_lcss(tree_t* tree)
{
    post_process_tree(tree);
    process_leaves_pair(tree);
    compute_uniq_str_count(tree);

    return get_lcss(tree);
}

void print_lcss(tree_t *tree, node_t **lcss)
{
    for (int i = 1; i <= tree->num_strings; i++)
    {
        char *i_lcs = to_string(tree, lcss[i]);
        printf("%d %s\n", i, i_lcs);
    }
}

bool check_end_sym(char end_sym)
{
    bool check = is_end_sym(end_sym);

    if (!check)
    {
        fprintf(stderr, "Too many strings\n");
        fprintf(stderr, "I can't handle more than %d strings :(\n", UCHAR_MAX - SCHAR_MAX - 1);
        fprintf(stderr, "(char '%d' isn't a valid end symbol)\n", end_sym);
    }

    return check;
}

joined_str_t *join_str_arr(char **strings)
{
    size_t total_len = 1; // +1 for '\0'
    size_t n_str;

    for (n_str = 0; strings[n_str] != NULL; n_str++)
    {
        // +1 for str unique end symbol
        total_len += strlen(strings[n_str]) + 1;
    }

    char *cat = calloc(total_len, 1); // sizeof(char)
    size_t cat_idx = 0;
    char end_sym = UCHAR_MAX;

    for (n_str = 0; strings[n_str] != NULL; n_str++)
    {
        char *cur_str = strings[n_str];
        size_t j = 0;

        while (cur_str[j] != '\0')
        {
            cat[cat_idx] = cur_str[j];

            cat_idx++;
            j++;
        }

        cat[cat_idx++] = end_sym--; // TODO (?)

        if (!check_end_sym(end_sym))
        {
            return NULL;
        }
    }

    // don't forget to bring a towel
    cat[cat_idx] = '\0';

    joined_str_t *res = malloc(sizeof(joined_str_t));
    res->ptr = cat;
    res->num_strings = n_str;

    return res;
}

void test_with_static_strings(void)
{
    char *test[] = {
        "asd",
        "asd",
        "asd",
        NULL
    };

    joined_str_t *join = join_str_arr(test);

    tree_t *tree = build_tree(join->ptr, join->num_strings);
    node_t **lcss = tree_to_lcss(tree);
    print_lcss(tree, lcss);
}

#define BASE_SIZE 1024

joined_str_t *read_strings(FILE* fp)
{
    size_t cur_size = BASE_SIZE;
    size_t num_strings = 1;
    size_t len = 0;
    char end_sym = UCHAR_MAX;
    char cur_c;

    char *str = malloc(cur_size); // sizeof(char)
    char *tmp = NULL;

    if (str == NULL)
    {
        return NULL;
    }

    while (true)
    {
        cur_c = fgetc(fp);

        if (cur_c == EOF)
        {
            break;
        }

        if (cur_c == '\n')
        {
            str[len++] = end_sym--;
            num_strings++;

            if (!check_end_sym(end_sym))
            {
                free(str);
                return NULL;
            }
        }
        else
        {
            str[len++] = cur_c;
        }

        if (len == cur_size)
        {
            cur_size *= 2;
            tmp = malloc(cur_size);

            memcpy(tmp, str, len);
            free(str);
            str = tmp;

            if (str == NULL)
            {
                return NULL;
            }
        }
    }

    str[len++] = '\0';

    joined_str_t *res = malloc(sizeof(joined_str_t));
    res->num_strings = num_strings;
    res->ptr = realloc(str, len); // sizeof(char)

    return res;
}

int main(void)
{
    joined_str_t *join = read_strings(stdin);

    if (join == NULL)
    {
        fprintf(stderr, "Error in read_strings\n");
        return 1;
    }

    tree_t *tree = build_tree(join->ptr, join->num_strings);
    node_t **lcss = tree_to_lcss(tree);
    print_lcss(tree, lcss);
}
