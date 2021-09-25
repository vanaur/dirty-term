#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#ifdef _WIN32
#include <conio.h>
#elif defined __linux__
#include <termios.h>

static struct termios old, current;

void initTermios(int echo)
{
    tcgetattr(0, &old);
    current = old;
    current.c_lflag &= ~ICANON;

    if (echo)
        current.c_lflag |= ECHO;
    else
        current.c_lflag &= ~ECHO;

    tcsetattr(0, TCSANOW, &current);
}

void resetTermios(void)
{
    tcsetattr(0, TCSANOW, &old);
}

char getch_(int echo)
{
    char ch;
    initTermios(echo);
    ch = getchar();
    resetTermios();
    return ch;
}

char getch(void)
{
    return getch_(0);
}

char getche(void)
{
    return getch_(1);
}
#endif

// Defines whether the input string starts with the given prefix and returns its length
int dtprefix(const char *str, const char *pre)
{
    int i = 0;
    while (*pre)
    {
        if (*pre++ != *str++)
            return 0;
        i += 1;
    }
    return i;
}

// The few keyboard constants that are useful
enum CMD_ARROW_KEY
{
    ARROW_UP =    'A',
    ARROW_DOWN =  'B',
    ARROW_LEFT =  'C',
    ARROW_RIGHT = 'D',
    KEY_TAB =     '\t'
};

// Return of user input
struct dtinput_t
{
    char *input; // The input
    size_t len;  // The length of the input
};

// Releases the memory used by the user input
void release_dtinput(struct dtinput_t *input)
{
    assert(input != NULL);
    free(input->input);
}

// Allows automatic completion of user entries
void auto_complete(const char prompt[], char **buff, const char **list,
                   const size_t len)
{
    if (!len) return;

    assert(buff != NULL);
    assert(*buff != NULL);
    assert(list != NULL);

    const size_t size = len * sizeof(int);

    int *indexes = malloc(size);
    memset(indexes, -1, size);

    int *pres = malloc(size);
    memset(pres, -1, size);

    const char *tok = strrchr(*buff, ' ');

    for (int i = 0; i < len; i++)
    {
        const int pre = dtprefix(list[i], !tok ? *buff : tok + 1);
        if (pre)
        {
            indexes[i] = i;
            pres[i] = pre;
        }
    }

    int nDiff = 0;
    int fst = 0;

    for (int i = 0; i < len; i++)
    {
        if (indexes[i] != -1)
        {
            nDiff++;
            fst = i;
        }
    }

    if (nDiff == 1)
    {
        strcat(*buff, list[fst] + pres[fst]);
        printf("%s", list[fst] + pres[fst]);
        goto end;
    }

    if (nDiff == 0)
        goto end;

    printf("\n");
    for (int i = 0; i < len; i++)
        if (indexes[i] != -1)
            printf("\t%s\t", list[indexes[i]]);
    printf("\n%s  %s", !prompt ? "" : prompt, *buff);

end:

    free(pres);
    free(indexes);
}

// Data structure used for the input history
struct dthistory_t
{
    char **list; // List of user input
    size_t len;  // Length of the list
};

// Liberate the memory of the history
void release_dt_history(struct dthistory_t *histroy)
{
    assert(histroy != NULL);
    for (int i = 0; i < histroy->len; i++)
        free(histroy->list[i]);
    free(histroy->list);
}

// Set a new history
struct dthistory_t new_dthistory()
{
    return (struct dthistory_t){malloc(sizeof(char *)), 0};
}

// Adds the given entry to the history
void add_to_history(struct dthistory_t *history, const char *input)
{
    assert(history != NULL);
    history->list[history->len++] = strdup(input);
    history->list = realloc(history->list, (history->len + 1) * sizeof(char *));
}

// Clears the i characters from the terminal
void erase_term_buff(const int i)
{
    for (int j = 0; j < i; j++) printf("\b");
    for (int j = 0; j < i; j++) printf(" ");
    for (int j = 0; j < i; j++) printf("\b");
}

// Allows to move through the history
void move_history(int *i, char **buff, const int buff_size, const char *h)
{
    erase_term_buff(*i);
    const int hlen = strlen(h);
    printf("%s", h);
    memset(*buff, 0, buff_size);
    strcpy(*buff, h);
    *i = hlen;
}

// Returns the user input and handle the few features
struct dtinput_t get_input(const char prompt[], struct dthistory_t *history, const char **to_complete, const size_t len)
{
    const size_t buff_size = 250;
    char *buff = malloc(buff_size);
    memset(buff, 0, buff_size);
    int i = 0;
    int hist_index = history->len;

    printf("%s  ", !prompt ? "" : prompt);

    do
    {
        const char curr = getch();
        if (curr == '\n') break;
        if (curr == KEY_TAB)
        {
            auto_complete(prompt, &buff, to_complete, len);
            continue;
        }
        if (curr == (char)127) // Handle the DEL key
        {
            if (i == 0)
                continue;
            buff[--i] = 0;
            printf("\b \b");
            continue;
        }

        if (curr == '\033')
        {
            getch();
            switch (getch())
            {
            case ARROW_UP:
            {
                if (hist_index == 0)
                    continue;
                char *h = history->list[--hist_index];
                move_history(&i, &buff, buff_size, h);
                continue;
            }
            case ARROW_DOWN:
            {
                if (hist_index + 1 >= history->len)
                    continue;
                char *h = history->list[++hist_index];
                move_history(&i, &buff, buff_size, h);
                continue;
            }
            }
        }

        buff[i++] = curr;
        putchar(curr);

    } while (i < buff_size);

    return (struct dtinput_t){buff, i};
}
