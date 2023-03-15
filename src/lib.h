/*
Cathal O'Grady, 21442084 - Operating Systems Project 1
ca216/myshell

I understand that the University regards breaches of academic integrity and
plagiarism as grave and serious. I have read and understood the DCU Academic
Integrity and Plagiarism Policy. I accept the penalties that may be imposed
should I engage in practice or practices that breach this policy. I have
identified and included the source of all facts, ideas, opinions and
viewpoints of others in the assignment references. Direct quotations,
paraphrasing, discussion of ideas from books, journal articles, internet
sources, module text, or any other source whatsoever are acknowledged and the
sources cited are identified in the assignment references. I declare that
this material, which I now submit for assessment, is entirely my own work and
has not been taken from the work of others save and to the extent that such
work has been cited and acknowledged within the text of my work. I have used
the DCU library referencing guidelines (available at
https://www4.dcu.ie/library/classes_and_tutorials/citingreferencing.shtml
and/or the appropriate referencing system recommended in the assignment
guidelines and/or programme documentation. By signing this form or by
submitting material online I confirm that this assignment, or any part of it,
has not been previously submitted by me or any other person for assessment on
this or any other course of study By signing this form or by submitting
material for assessment online I confirm that I have read and understood the
DCU Academic Integrity and Plagiarism Policy (available at
http://www.dcu.ie/registry/examinations/index.shtml).
*/

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
        vector = temp;                                                        \
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
#define arr_length(arr) sizeof(arr) / sizeof(*arr)

#define foreach(T, e, object, length)                                          \
  for (T *_iter = object, e = *_iter; _iter != (object + length);              \
       *_iter++, e = *_iter)

#define loop for (;;)

void die(char *msg);
char *strip(char *start, char *end, const char *chars);
char *join(char **list, size_t length, char glue);
char *str_alloc(const char *s);
int empty(char *s);

#endif
