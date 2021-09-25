# dirty-term
Small and dirty header-only library that supports user input with some more advanced features than in the standard lib. This small, lightweight, cross-platform lib allows the user to enter the terminal with nicer features than `scanf`, `getline` and similar. It allow:

 - Input history with keyboard arrows
 - Auto-completion (with TAB) according to a dynamic list
 - Forward and backward editing of current input

It's not much, but often sufficient in most applications, and much lighter than a lib like ncurse! The library has been designed for REPL in mind (i.e. looping entries), but can of course be used for other purposes. Here is an example:

```c
#include "dirty-term.h"

int main()
{
   /* For auto-completion, we define here a static array containing
      all the words we want to be able to complete */

    const char *list_of_words[] =
        { "kitten",   "compuer",      "noob"
        , "kitchen",  "mesabloo",     "physics"
        , "java",     "music",        "photon"
        , "memory",   "programming",  "minecraft" };
    
    const size_t length = sizeof(list_of_words) / sizeof(char *);
    struct dthistory_t history = new_dthistory(); /* We introduce the history */
    
    while (1)
    {
        /* We take the user input */
        struct dtinput_t input = get_input(">>>", &history, list_of_words, length);
        /* If the previous entry is not empty, it is added to the history */
        if (input.input[0] != 0)
            add_to_history(&history, input.input);

        /* The memory if released */
        release_dtinput(&input);
        printf("\n"); /* When ENTER is pressed, it does not display the line break,
                         so we have to display it ourselves */
    }
    
    /* The history memory is released */
    release_dt_history(&history);

    return 0;
}
```

## The limits of lib and remarks

User input is retrieved via `input.input` and the size of the input via `input.len` where `input` is of type `struct dtinput_t`. The prompt can be modified to display the characters you want (see example). The maximum size of the entry is arbitrarily set to 250 characters, but you can change this in the lib. Once this limit is exceeded, the input does not wait for anything else and returns its result normally. Auto-completion is limited to the last **word** entered by the user, i.e. the last character sequence separated by a space. The lib wasn't designed for speed, anyway, unless your user is writing at 10000000 wpm, you shouldn't care.
