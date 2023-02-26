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
            die("Failed to realloc");                                         \
        }                                                                     \
        capacity = (capacity * CAPACITY_MULTIPLIER);                          \
        *((T *)vector + length) = item;                                       \
        length += 1;                                                          \
    }                                                                         \
}

/**
 * vec_print(T, T *vector, size_t length, const char *format_str)
 */
#define vec_print(T, vector, length, format_str) {                            \
    putchar('[');                                                             \
    for (size_t i = 0; i < length; ++i) {                                     \
        if (i != 0) {                                                         \
            printf(", ");                                                     \
        }                                                                     \
        printf(format_str, *((T *)vector + i));                               \
    }                                                                         \
    printf("]\n");                                                            \
}

#define bin_set_flag(flags, flag) flags |= flag
#define bin_unset_flag(flags, flag) flags &= ~flag
#define bin_isset_flag(flags, flag) ((flags & flag) == flag)
#define bin_toggle_flag(flags, flag) flags ^= flag

#define str_length(start, end) ((size_t)(end - start) / sizeof(char) + 1)

#define foreach(T, e, object, length)                                          \
  for (T *_iter = object, e = *_iter; _iter != (object + length);              \
       *_iter++, e = *_iter)

#define loop for (;;)

void die(char *msg);
char *strip(char *start, char *end, const char ch);
char *str_alloc(const char *s);

#endif
