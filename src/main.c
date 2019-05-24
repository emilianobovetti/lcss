#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#include "tree.h"
#include "ukkonen.h"

typedef struct joined_strs
{
    char *ptr;

    size_t num_strings;
}
joined_strs_t;

lcss_array_list_t **build_lcss(tree_t* tree)
{
    post_process_tree(tree);
    process_leaves_pair(tree);
    compute_uniq_str_count(tree);

    return get_lcss(tree);
}

void print_lcss(tree_t *tree, lcss_array_list_t **lcss)
{
    for (int i = 1; i <= tree->num_strings; i++)
    {
        printf("%d", i);

        lcss_array_list_t *lcss_node = lcss[i];

        while (lcss_node != NULL)
        {
            char *i_lcs = node_to_string(tree, lcss_node->current);
            printf(" %s", i_lcs);
            free(i_lcs);

            lcss_node = lcss_node->next;
        }

        putchar('\n');
    }
}

bool check_end_sym(char end_sym)
{
    bool check = is_end_sym(end_sym);

    if (!check)
    {
        fprintf(stderr, "Too many strings\n");
        fprintf(stderr, "I can't handle more than %d strings :(\n", UCHAR_MAX - SCHAR_MAX - 1);
    }

    return check;
}

joined_strs_t *join_str_arr(char **strings)
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

        cat[cat_idx++] = end_sym--;

        if (!check_end_sym(end_sym))
        {
            return NULL;
        }
    }

    // don't forget to bring a towel
    cat[cat_idx] = '\0';

    joined_strs_t *res = malloc(sizeof(joined_strs_t));
    res->ptr = cat;
    res->num_strings = n_str;

    return res;
}

void test_with_static_strings(void)
{
    char *test[] = {
        "test1",
        "test2",
        "test3",
        NULL
    };

    joined_strs_t *join = join_str_arr(test);

    tree_t *tree = build_tree(join->ptr, join->num_strings);
    lcss_array_list_t **lcss = build_lcss(tree);
    print_lcss(tree, lcss);
}

#define STR_SEP '\n'
#define BASE_SIZE 1024

joined_strs_t *read_strings(FILE* fp)
{
    size_t cur_size = BASE_SIZE;
    size_t num_strings = 1;
    size_t len = 0;
    char end_sym = UCHAR_MAX;
    char cur_c = '\0', last_c = '\0';

    char *str = malloc(cur_size); // sizeof(char)
    char *tmp = NULL;

    if (str == NULL)
    {
        return NULL;
    }

    while (true)
    {
        last_c = cur_c;
        cur_c = fgetc(fp);

        if (cur_c == EOF)
        {
            if (last_c == STR_SEP)
            {
                num_strings--;
            }

            break;
        }

        if (cur_c == STR_SEP)
        {
            if (last_c == STR_SEP)
            {
                continue;
            }

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

    joined_strs_t *res = malloc(sizeof(joined_strs_t));
    res->num_strings = num_strings;
    res->ptr = realloc(str, len); // sizeof(char)

    return res;
}

int main(void)
{
    joined_strs_t *join = read_strings(stdin);

    if (join == NULL)
    {
        fprintf(stderr, "Error in read_strings\n");
        return 1;
    }

    tree_t *tree = build_tree(join->ptr, join->num_strings);
    lcss_array_list_t **lcss = build_lcss(tree);
    print_lcss(tree, lcss);
}
