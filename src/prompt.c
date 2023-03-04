#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "prompt.h"
#include "lib.h"

static void get_current_path();

drawer prompt_expand_character[] = {
    ['p'] = get_current_path,
};

static void
get_current_path()
{
    printf("%s", getenv("PWD"));
    fflush(stdout);
}


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

