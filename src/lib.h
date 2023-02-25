#ifndef LIB_H
#define LIB_H

#include <stdlib.h>
#include <stdio.h>

/* Generic Vector Macros */

/**
 * CAPACITY_MULTIPLIER: Define how much capacity should increase by when it
 * runs out
 */
#define CAPACITY_MULTIPLIER 2

/**
 * vec_push_back(T, T *vector, size_t capacity, size_t length, T item)
 */
#define vec_push_back(T, vector, capacity, length, item) {                    \
    T *temp;                                                                  \
    if (length < capacity) {                                                  \
        *((T *)vector + length) = item;                                       \
        length += 1;                                                          \
    } else {                                                                  \
        temp = realloc(                                                       \
            vector, (capacity * CAPACITY_MULTIPLIER) * sizeof(T));            \
        if (!temp) {                                                          \
            free(vector);                                                     \
            die("Failed to realloc");                                         \
        }                                                                     \
        *((T *)vector + length) = item;                                       \
        length += 1;                                                          \
    }                                                                         \
}

void die(char *msg);

#endif
