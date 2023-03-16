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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "prompt.h"
#include "lib.h"

static void get_current_path();

drawer prompt_expand_character[] = {
    ['p'] = get_current_path,
};

/**
 * [[helper]] draws the current path
 */
static void
get_current_path()
{
    printf("%s", getenv("PWD"));
    fflush(stdout);
}


/**
 * Draws the prompt to the console
 */
void
draw_prompt()
{
    char *prompt_unformatted;
    size_t prompt_unformatted_length;
    drawer expander;
    size_t drawer_range;
    char format_character;
    int in_range;
    char character;
    char last_character;

    drawer_range = arr_length(prompt_expand_character);

    prompt_unformatted = getenv("prompt");
    prompt_unformatted_length = strlen(prompt_unformatted);

    /* look for special character and execute associated function */
    for (size_t i = 0; i < prompt_unformatted_length - 1; i += 1) {
        character = prompt_unformatted[i];
        if (character == SPECIAL_PROMPT_CHARACTER) {
            format_character = prompt_unformatted[++i];
            in_range = format_character <= drawer_range;

            if (in_range) {
                expander = prompt_expand_character[format_character];
                if (expander) expander();
                continue;
            }
        }
        putchar(character);
    }

    last_character = prompt_unformatted[prompt_unformatted_length - 1];
    putchar(last_character);
}
