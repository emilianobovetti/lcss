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

void print_node_arr_lst(tree_t *tree, node_list_t **arr_lst)
{
    for (int i = 1; i <= tree->num_strings; i++)
    {
        printf("%d", i);

        node_list_t *node_lst = arr_lst[i];

        while (node_lst != NULL)
        {
            char *i_str = node_to_string(tree, node_lst->current);
            printf(" %s", i_str);
            free(i_str);

            node_lst = node_lst->next;
        }

        putchar('\n');
    }
}

/*
 * Checks if a character is an end symbol.
 */
bool check_end_sym(char end_sym)
{
    bool check = is_end_sym(end_sym);

    if (!check)
    {
        fprintf(stderr, "Too many strings\n");
        fprintf(stderr, "Can't handle more than %d strings\n", UCHAR_MAX - SCHAR_MAX - 1);
    }

    return check;
}

/*
 * This function is here for testing purpose, not actually used.
 */
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

    cat[cat_idx] = '\0';

    joined_strs_t *res = malloc(sizeof(joined_strs_t));
    res->ptr = cat;
    res->num_strings = n_str;

    return res;
}

/*
 * This function is here for testing purpose, not actually used.
 */
void test_with_static_strings(void)
{
    char *test[] = {
        "test1",
        "test2",
        "test3",
        NULL
    };

    int range = 0;

    joined_strs_t *join = join_str_arr(test);

    tree_t *tree = build_tree(join->ptr, join->num_strings);
    post_process_tree(tree);
    node_list_t **lcss = get_commons_by_length(tree, INT_MAX, range);

    print_node_arr_lst(tree, lcss);
}

#define STR_SEP '\n'
#define BASE_SIZE 1024

/*
 * Reads strings from a file.
 * Every string is terminated by STR_SEP.
 * Copies the strings in the heap, adds end symbols.
 * Returns a pointer to the resulting string
 * and the number of the total strings.
 */
joined_strs_t *read_strings(FILE* fp)
{
    size_t cur_size = BASE_SIZE;
    size_t num_strings = 1;
    size_t len = 0;

    /*
     * The way end symbols are choosen sets a limit of max 127
     * input strings.
     * This is due to a naive implementation of generalized
     * suffix tree.
     *
     * Anyway don't just change this number, you could break
     * 'get_leaf_str_idx' function.
     * That function must guarantee that every string has a
     * unique index that goes from 0 to num_strings.
     *
     * So the way in which those symbols are assigned is
     * tightly coupled to 'get_leaf_str_idx'.
     *
     * This looks like a bad idea, but if you need to
     * process more than 127 strings it's likely you'll need
     * a better approach to generalized suffix tree anyway.
     */
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
                /*
                 * The input ends with a STR_SEP, the actual
                 * number of total strings must be decreased.
                 */
                num_strings--;
            }

            break;
        }

        if (cur_c == STR_SEP)
        {
            if (last_c == STR_SEP)
            {
                /*
                 * That's an empty string.
                 */
                continue;
            }

            str[len++] = end_sym--;
            num_strings++;

            if (!check_end_sym(end_sym))
            {
                /*
                 * The current end symbol is invalid.
                 * There are too many strings.
                 */
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

    joined_strs_t *res = malloc(sizeof(joined_strs_t)); // shrink down
    res->num_strings = num_strings;
    res->ptr = realloc(str, len); // sizeof(char)

    return res;
}

typedef enum selection_mode
{
    LENGTH, MELTING_POINT
}
selection_mode_t;

#define STR_EQUAL(s1, s2) (strcmp((s1), (s2)) == 0)

int main(int argc, char **argv)
{
    selection_mode_t mode = LENGTH;
    int target_melt_pt = 0;
    int target_len = INT_MAX;
    int range = 0;

    for (int i = 1; i < argc; i++)
    {
        char *cur_arg = argv[i];

        if (STR_EQUAL("-l", cur_arg) || STR_EQUAL("--length", cur_arg))
        {
            mode = LENGTH;

            if (i + 1 < argc)
            {
                target_len = strtol(argv[i + 1], NULL, 10);
            }

            if (target_len == 0)
            {
                target_len = INT_MAX;
            }
            else if (target_len < INT_MAX)
            {
                i++;
            }
        }
        else if (STR_EQUAL("-m", cur_arg) || STR_EQUAL("--melting-point", cur_arg))
        {
            mode = MELTING_POINT;
            i++;

            if (i == argc)
            {
                fprintf(stderr, "please specify a melting point\n");
                return 1;
            }

            target_melt_pt = strtol(argv[i], NULL, 10);
        }
        else if (STR_EQUAL("-r", cur_arg) || STR_EQUAL("--range", cur_arg))
        {
            i++;

            if (i == argc)
            {
                fprintf(stderr, "please specify a range\n");
                return 1;
            }

            range = strtol(argv[i], NULL, 10);
        }
        else
        {
            fprintf(stderr, "unrecognized argument '%s' in %s\n", cur_arg, argv[0]);
            return 1;
        }
    }

    joined_strs_t *join = read_strings(stdin);

    if (join == NULL)
    {
        fprintf(stderr, "error in read_strings\n");
        return 1;
    }

    tree_t *tree = build_tree(join->ptr, join->num_strings);
    post_process_tree(tree);

    node_list_t **res_arr_lst;

    switch (mode)
    {
        case LENGTH:
            res_arr_lst = get_commons_by_length(tree, target_len, range);
            break;
        case MELTING_POINT:
            res_arr_lst = get_commons_by_melting_point(tree, target_melt_pt, range);
            break;
    }

    print_node_arr_lst(tree, res_arr_lst);
}
