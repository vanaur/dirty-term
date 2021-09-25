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
