#ifndef __SUBSTRING_H__
#define __SUBSTRING_H__

#include <stdlib.h>

/*
 * Given an array `a` and a pair of indexes `left` and `right`,
 * a substring is defined as `a[left]..a[right]`.
 *
 * `left = right = 0` represent a single-char substring (a[left] = a[right]).
 *
 * Any `left > right` represent an empty string.
 */
typedef struct substring
{
    size_t left, right;
}
substring_t;

#define SUBSTRING_IS_EMPTY(s) ((s).left > (s).right)

#endif
